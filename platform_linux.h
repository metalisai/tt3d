#ifndef PLATFORM_LINUX_H
#define PLATFORM_LINUX_H

#define GL_GLEXT_PROTOTYPES
#define GLX_GLXEXT_PROTOTYPES

#include <X11/Xlib.h>

#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glx.h>
#include <GL/glu.h>

//#include "core.h"
//#include "core.hpp"
#include "engine_platform.h"

typedef struct PlatformState
{
    Display *display;
    Window win;
} PlatformState;

typedef struct LinuxFileHandle
{
    int fileHandle;
} LinuxFileHandle;

Window Linux_CreateWindow(Display *display, int width, int height, const char* windowTitle);

#endif // PLATFORM_LINUX_H
