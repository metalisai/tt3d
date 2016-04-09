#include "platform_linux.h"

#include "types.h"
#include "ttMath.h"

#include "input.h"

#include <dlfcn.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <sys/stat.h>
#include <unistd.h>

int (*functionPtr)(int,int);

void (*init    ) (MemStack* gamemem, int width, int height);
void (*display ) (MemStack* gamemem, Input* input, float dt);
void (*keyboard) ( unsigned char key, int mouseX, int mouseY );
void (*pMouse  ) (int x, int y);
void (*reshape ) (MemStack* game_memory, int w, int h);

void* dlHandle = 0;
timespec lastModified;
const char* codeFile = "./libgame.so";

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

    init        = (void(*)(MemStack* gamemem, int width, int height))       dlsym(dlHandle, "init");
    display     = (void(*)(MemStack* gamemem, Input* input, float dt))      dlsym(dlHandle, "display");
    keyboard    = (void(*)( unsigned char key, int mouseX, int mouseY ))    dlsym(dlHandle, "keyboard");
    pMouse      = (void(*)(int x, int y))                                   dlsym(dlHandle, "pMouse");
    reshape     = (void(*)(MemStack* game_memory, int w, int h))            dlsym(dlHandle, "reshape");
}

int main()
{
    loadCode();

    MemStack gameMemory;
    size_t size = 50*1024LL*1024LL;
    void* gmem = malloc(size); // 50MB
    stackInit(&gameMemory, gmem, size);

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
    init(&gameMemory,1024,768);
    reshape(&gameMemory,1024,768);
    Atom wmDeleteMessage = XInternAtom(state.display, "WM_DELETE_WINDOW", False);
    XSetWMProtocols(state.display, state.win, &wmDeleteMessage, True);

    if(reshape == 0)
        printf("getting reshape failed!\n");
    /*if ((error = dlerror()) != NULL)  {
        fputs(error, stderr);
        exit(1);
    }*/

    XEvent event;
    bool running = true;
    timespec start;
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
                XConfigureEvent xce = event.xconfigure;
                // TODO: this event might not always be a resize event?
                reshape(&gameMemory,xce.width,xce.height);
                break;
            }
        }

        timespec last = start;
        clock_gettime(CLOCK_MONOTONIC, &start);
        float seconds = start.tv_sec-last.tv_sec+(start.tv_nsec-last.tv_nsec)/1000000000.0;
        display(&gameMemory, &input, seconds);
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
