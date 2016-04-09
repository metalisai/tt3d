#ifndef MATERIAL_H
#define MATERIAL_H

#include <GL/gl.h>
#include "types.h"
#include "shader.h"

#define MAX_TEXTURES 2

struct Material
{
    Shader* shader;
    GLuint texture_handle[MAX_TEXTURES];
    u16 numTextures;
};

#endif // MATERIAL_H
