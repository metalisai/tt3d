#include "renderer.h"

#define STB_IMAGE_IMPLEMENTATION
#include "lib/stb_image.h"

TexturePool* createTexturePool(i32 maxTextures, MemoryArena* stack)
{
    // TODO: use some stack allocator
    TexturePool* ret = (TexturePool*)malloc(sizeof(TexturePool));
    ret->beginAddr = (LoadedTexture*)arenaPushSize(stack,sizeof(LoadedTexture)*maxTextures);
    for(int i = 0; i < maxTextures; i++)
    {
        ret->beginAddr[i].used = false;
    }
    ret->size = maxTextures;
    return ret;
}

LoadedTexture* getFreeTexture(TexturePool* pool)
{
    // TODO: linear search could be optimized for something better
    for(int i = 0; i < pool->size; i++)
    {
        if(!pool->beginAddr[i].used)
            return &pool->beginAddr[i];
    }
    return 0;
}

LoadedTexture* loadTextureToMemory(TexturePool* pool, const char* filename)
{
    LoadedTexture* tex = getFreeTexture(pool);
    int n = 0;
    tex->data = stbi_load(filename,&tex->width,&tex->height,&n,3);
    if(tex->data == 0)
    {
        printf("Could not load texture %s\n",filename);
        return 0;
    }
    tex->used = true;
    tex->boundToGPU = false;
    return tex;
}
