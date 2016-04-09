#ifndef TEXTURE_H
#define TEXTURE_H

#include <GL/glew.h>
#include "types.h"
#include "memory.h"

struct LoadedTexture
{
    GLuint gl_handle;
    int width;
    int height;
    bool used;
    bool boundToGPU;
    void* data;
};

struct TexturePool
{
    LoadedTexture* beginAddr;
    int size;
};

TexturePool* createTexturePool(i32 maxTextures, MemStack* stack);
LoadedTexture* loadTextureToMemory(TexturePool* pool, const char* filename);
void openGL_LoadTexture(LoadedTexture* tex);

#endif // TEXTURE_H
