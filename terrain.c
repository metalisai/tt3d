#include "engine.h"

/*#define STB_PERLIN_IMPLEMENTATION
#include "lib/stb_perlin.h"*/

Mesh* generateTerrainMesh()
{
    int width = 100;
    int height = 100;

    int vertexSize = width*height*sizeof(Vec4);
    int normalSize = width*height*sizeof(Vec3);
    int texSize = width*height*sizeof(Vec2);
    int indexSize = ((width-1) * (height-1) * 6 * sizeof(u16));
    int tangentSize = width*height*sizeof(Vec3);
    const void* data = malloc(vertexSize+normalSize+indexSize+texSize+tangentSize);

    Vec4* vertices = (Vec4*)data;

    char* data2 = (char*)data;
    Vec3* normals = (Vec3*)((data2+width*height*sizeof(Vec4)));
    Vec2* uv = (Vec2*)(normals + width*height);
    data2 += width*height*sizeof(Vec4)+width*height*sizeof(Vec3)+width*height*sizeof(Vec2);
    u16* triangles = (u16*)data2;
    //Vec2[] uv = new Vec2[width * height];

    //memzero()

    int indices = 0;
    r32 scale = 1.0f;

    for(int i = 0; i < width; i++)
    {
        for(int j = 0; j < height; j++)
        {
            vertices[j + i * height] = vec4(j*scale, /*stb_perlin_noise3((r32)j / 10.0f, (r32)i / 10.0f, 0.0f)*scale*5.0f*/0.0f, i*scale, 1.0f);
            uv[j + i * height] = vec2(j % 2 == 1 ? 1.f : 0.f, i % 2 == 1 ? 1.f : 0.f);

            if (i != (height-1) && j != (width-1))
            {
                triangles[indices] = j + i * height;
                triangles[indices + 2] = (j + 1) + (i + 1) * height;
                triangles[indices + 1] = j + (i + 1) * height;
                triangles[indices + 3] = j + i * height;
                triangles[indices + 5] = (j + 1) + i * height;
                triangles[indices + 4] = (j + 1) + (i + 1) * height;
                indices += 6;
            }
        }
    }
    for(int i = 0; i < width*height;i++)
        normals[i] = vec3(1.0f,0.0f,0.0f);

    Mesh* mesh = malloc(sizeof(Mesh));
    meshInit(mesh);
    mesh->cold->vertices = width*height;
    mesh->cold->normals = width*height;
    mesh->faces = (width-1) * (height-1)*2;
    mesh->cold->data = (void*)data;
    meshRecalculateBounds(mesh);
    return mesh;
}
