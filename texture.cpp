#include "renderer.h"

#define STB_IMAGE_IMPLEMENTATION
#include "lib/stb_image.h"

TexturePool* createTexturePool(i32 maxTextures, MemStack* stack)
{
    // TODO: use some stack allocator
    TexturePool* ret = new TexturePool;
    ret->beginAddr = (LoadedTexture*)stackPushSize(stack,sizeof(LoadedTexture)*maxTextures);
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

void openGL_LoadTexture(LoadedTexture* tex)
{
    glGenTextures(1, &tex->gl_handle);
    glBindTexture(GL_TEXTURE_2D, tex->gl_handle);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, tex->width, tex->height, 0, GL_RGB,
                  GL_UNSIGNED_BYTE, tex->data);
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);  
    tex->boundToGPU = true;
}
