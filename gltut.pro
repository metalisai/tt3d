TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt
CONFIG += c++11

HEADERS += stb_perlin.h \
    stb_image.h \
    platform_linux.h \
    core.h \
    renderer.h \
    engine.h \
    shared.h \
    engine_platform.h \
    opencl.h

SOURCES += \
    platform_linux.c \
    ttmath.c \
    memory.c \
    input.c \
    transform.c \
    material.c \
    camera.c \
    texture.c \
    debug.c \
    mesh.c \
    modelParser.c \
    terrain.c \
    core.c \
    audio.c \
    opengl.c \
    voxel_terrain.c \
    renderer.c

LIBS += -lGL
LIBS += -lGLEW
LIBS += -lX11
LIBS += -lfreetype
LIBS += -lopenal
LIBS += -lalut

 INCLUDEPATH += /usr/include/freetype2 \

