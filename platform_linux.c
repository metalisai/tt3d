#include "platform_linux.h"

#include "shared.h"

#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include <dlfcn.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <signal.h>


int (*functionPtr)(int,int);

void (*init    ) (EngineMemory* gamemem, int width, int height);
void (*display ) (EngineMemory* gamemem, Input* input, float dt);
void (*keyboard) ( unsigned char key, int mouseX, int mouseY );
void (*pMouse  ) (int x, int y);
void (*reshape ) (EngineMemory* game_memory, int w, int h);

void* dlHandle = 0;
struct timespec lastModified;
const char* codeFile = "./libgame.so";

static PLATFORM_OPEN_FILE(linuxOpenFile) // macro defined in engine_platform.h
{
    PlatformFileHandle result = {};

    LinuxFileHandle* handle = (LinuxFileHandle*)malloc(sizeof(LinuxFileHandle));
    handle->fileHandle = open(fileName, O_RDONLY, S_IREAD);
    if(handle->fileHandle > 0)
    {
        result.noErrors = true;
        result.PlatformData = handle;
    }
    else
    {
        result.noErrors = false;
    }

    return result;
}

static PLATFORM_GET_FILE_SIZE(linuxGetFileSize)
{
    LinuxFileHandle* fhandle = (LinuxFileHandle*)fh->PlatformData;
    struct stat fileStat;
    int statRes = fstat(fhandle->fileHandle, &fileStat);
    if(statRes >= 0)
    {
        return fileStat.st_size;
    }
    return -1;
}

static PLATFORM_READ_DATA_FROM_FILE(linuxReadFromFile)
{
    LinuxFileHandle* fhandle = (LinuxFileHandle*)source->PlatformData;
    int res = lseek(fhandle->fileHandle, offset, SEEK_SET);
    if(res != -1)
    {
        res = read(fhandle->fileHandle, dest, size);
        if(res != -1)
            return;
    }
    printf("Error when reading from while\n");
    source->noErrors = false;
}

static PLATFORM_CLOSE_FILE(linuxCloseFile) // macro defined in engine_platform.h
{
    LinuxFileHandle* fhandle = (LinuxFileHandle*)handle->PlatformData;
    close(fhandle->fileHandle);
    free((void*)fhandle);
}

void loadCode()
{
    if(dlHandle != 0)
    {
        //return;
        printf("Realoading game code!\n");
        int closed = dlclose(dlHandle);
        if(closed != 0)
            printf("Unloading game code failed! %s\n",dlerror());
    }

    dlHandle = dlopen (codeFile, RTLD_LAZY);
    while(!dlHandle) {
        printf("Could not load libgame.so, retrying! %s\n",dlerror());
        usleep(1000000);
        dlHandle = dlopen (codeFile, RTLD_LAZY);
    }

    struct stat attrib;
    stat(codeFile, &attrib);
    lastModified = attrib.st_mtim;
    init        = (void(*)(EngineMemory* gamemem, int width, int height))       dlsym(dlHandle, "init");
    display     = (void(*)(EngineMemory* gamemem, Input* input, float dt))      dlsym(dlHandle, "display");
    keyboard    = (void(*)( unsigned char key, int mouseX, int mouseY ))    dlsym(dlHandle, "keyboard");
    pMouse      = (void(*)(int x, int y))                                   dlsym(dlHandle, "pMouse");
    reshape     = (void(*)(EngineMemory* game_memory, int w, int h))            dlsym(dlHandle, "reshape");
    //dlclose(codeFile);

}

int main()
{
    loadCode();

    //MemStack gameMemory;
    size_t size = 200LL*1024LL*1024LL;
    EngineMemory* eMem = (EngineMemory*)malloc(size); // 50MB
    eMem->gameState = (struct GameState*)((char*)eMem+sizeof(EngineMemory));
    eMem->transientState = (struct TransientState*)((char*)eMem->gameState+60LL*1024LL*1024LL);
    eMem->platformApi.openFile      = (platformOpenFile*) linuxOpenFile;
    eMem->platformApi.closeFile     = (platformCloseFile*) linuxCloseFile;
    eMem->platformApi.readFromFile  = (platformReadFromFile*) linuxReadFromFile;
    eMem->platformApi.getFileSize   = (platformGetFileSize*) linuxGetFileSize;
    //stackInit((MemStack*)eMem->gameState, ((char*)eMem->gameState)+sizeof(MemStack), 60LL*1024LL*1024LL-sizeof(MemStack));

    Input input;
    memset(&input, 0, sizeof(input));

    PlatformState state;
    state.display = XOpenDisplay(":0.0");
    state.win = Linux_CreateWindow(state.display, 1024, 768, "Engine?");
    if(glewInit() != GLEW_OK)
    {
        printf("Glew Init failed!\n");
        return 0;
    }
    if (glewIsSupported("GL_VERSION_4_3"))
    {
        printf("GL 4.3 Supported \n");
    }
    else
    {
        printf("GL 4.3 NOT Supported, exiting!\n");
        return -1;
    }
    init(eMem,1024,768);
    reshape(eMem,1024,768);
    Atom wmDeleteMessage = XInternAtom(state.display, "WM_DELETE_WINDOW", False);
    XSetWMProtocols(state.display, state.win, &wmDeleteMessage, True);

    if(reshape == 0)
        printf("getting reshape failed!\n");
    /*if ((error = dlerror()) != NULL)  {
        fputs(error, stderr);
        exit(1);
    }*/

    XEvent event;
    b32 running = true;
    struct timespec start;
    clock_gettime(CLOCK_MONOTONIC, &start);
    while(running)
    {
        while (XPending(state.display) > 0)
        {
            // Fetch next event:
            XNextEvent(state.display, &event);
            char buffer[20];
            int bufsize = 20;
            KeySym key;
            XComposeStatus compose;
            //int charcount;
            // Process the event:
            switch (event.type) {
            case ClientMessage:
                if (event.xclient.data.l[0] == (long)wmDeleteMessage)
                {
                    printf("Delete message! Exiting...\n");
                    running = false;
                }
                break;
            case MotionNotify:
                input.mousePosition.x = event.xmotion.x;
                input.mousePosition.y = event.xmotion.y;
                //pMouse(event.xmotion.x,event.xmotion.y);
                break;
            case KeyPress:
                {
                    // TODO: ther could be multiple characters?
                    /*charcount = */XLookupString((XKeyEvent*)&event, buffer, bufsize, &key, &compose);
                    int keycode = charToKeycode(buffer[0]);
                    input.keyStates[keycode] = 1;;
                }
                break;
            case KeyRelease:
                {
                    /*charcount = */XLookupString((XKeyEvent*)&event, buffer, bufsize, &key, &compose);
                    int keycode = charToKeycode(buffer[0]);
                    input.keyStates[keycode] = 0;
                }
                break;
            case ConfigureNotify:
                {
                    XConfigureEvent xce = event.xconfigure;
                    // TODO: this event might not always be a resize event?
                    reshape(eMem,xce.width,xce.height);
                }
                break;
            }
        }

        struct timespec last = start;
        clock_gettime(CLOCK_MONOTONIC, &start);
        float seconds = start.tv_sec-last.tv_sec+(start.tv_nsec-last.tv_nsec)/1000000000.0;
        display(eMem, &input, seconds);
        glXSwapBuffers(state.display, state.win);

        for(int i = 0; i < KEY_COUNT; i++)
        {
            input.keyStatesLastFrame[i] = input.keyStates[i];
        }

        struct stat attrib;
        stat(codeFile, &attrib);
        if(lastModified.tv_sec != attrib.st_mtim.tv_sec && lastModified.tv_nsec != attrib.st_mtim.tv_nsec)
            loadCode();
    }
    dlclose(dlHandle);
    return 0;
}

Window Linux_CreateWindow(Display* display, int width, int height, const char* windowTitle)
{
    if (!display )
        printf( "Cannot open X display\n" );
    int screen = DefaultScreen( display );
    Window root_win = RootWindow( display, screen );
    if ( !glXQueryExtension( display, 0, 0 ) )
        printf( "X Server doesn't support GLX extension\n" );
    int attrib_list[] = {
        GLX_RENDER_TYPE, GLX_RGBA_BIT,
        GLX_RED_SIZE, 8,
        GLX_GREEN_SIZE, 8,
        GLX_BLUE_SIZE, 8,
        GLX_DEPTH_SIZE, 24,
        GLX_DOUBLEBUFFER, True,
        GLX_SAMPLE_BUFFERS  , 1,
        GLX_SAMPLES         , 4,
        None};
    int nElements = 0;
    GLXFBConfig* fbconfig = glXChooseFBConfig( display, screen, attrib_list, &nElements);
    if ( !fbconfig )
    {
        printf( "Failed to get GLXFBConfig\n" );
        exit(1);
    }
    XVisualInfo *visinfo = glXGetVisualFromFBConfig( display, *fbconfig );
    if ( !visinfo )
    {
        printf( "Failed to get XVisualInfo\n" );
        exit(1);
    }
    // Create the X window
    XSetWindowAttributes winAttr ;
    winAttr.event_mask = StructureNotifyMask | KeyPressMask| KeyReleaseMask | PointerMotionMask;
    winAttr.background_pixmap = None ;
    winAttr.background_pixel  = 0    ;
    winAttr.border_pixel      = 0    ;
    winAttr.colormap = XCreateColormap( display, root_win,
                                        visinfo->visual, AllocNone );
    unsigned int mask = CWBackPixmap | CWBorderPixel | CWColormap | CWEventMask;
    const int   WIN_XPOS    = 150;
    const int   WIN_YPOS    = 150;
    Window win = XCreateWindow (display, root_win,
                                WIN_XPOS, WIN_YPOS,
                                width, height, 0,
                                visinfo->depth, InputOutput,
                                visinfo->visual, mask, &winAttr ) ;
    XStoreName( display, win, windowTitle);
    // Create an OpenGL context and attach it to our X window
    GLXContext context = glXCreateContext( display, visinfo, NULL, GL_TRUE);
    if (!glXMakeCurrent(display, win, context))
        printf("glXMakeCurrent failed.\n");
    if (!glXIsDirect(display,glXGetCurrentContext()))
        printf( "Indirect GLX rendering context obtained\n");
    XMapWindow(display, win);
    if (!glXMakeCurrent(display,win,context))
        printf("glXMakeCurrent failed.\n");
    glViewport  ( 0,0,width,height);
    glColorMask ( 1,1,1,1 );
    glClearColor( 0,0,1,1 );
    glClear     ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    return win;
}


