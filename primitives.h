#ifndef PRIMITIVES_H
#define PRIMITIVES_H

#include "mesh.h"

Mesh* generatePlane()
{
    int vertexSize = 4*sizeof(Vec4);
    int normalSize = 4*sizeof(Vec3);
    int texSize = 4*sizeof(Vec2);
    int indexSize = (6 * sizeof(u16));
    const void* data = malloc(vertexSize+normalSize+indexSize+texSize);

    Vec4* vertices = (Vec4*)data;

    char* data2 = (char*)data;
    Vec3* normals = (Vec3*)((data2+4*sizeof(Vec4)));
    Vec2* uv = (Vec2*)(normals + 4);
    data2 += 4*sizeof(Vec4)+4*sizeof(Vec3)+4*sizeof(Vec2);
    u16* triangles = (u16*)data2;

    vertices[0] =  Vec4(-0.5f,-0.5f,-0.0f,1.0f);
    vertices[1] =  Vec4(0.5f,-0.5f,-0.0f ,1.0f);
    vertices[2] =  Vec4(0.5f,0.5f,0.0f  ,1.0f);
    vertices[3] =  Vec4(-0.5f,0.5f,0.0f ,1.0f);

    uv[0] = Vec2(0.0f,0.0f);
    uv[1] = Vec2(1.0f,0.0f);
    uv[2] = Vec2(1.0f,1.0f);
    uv[3] = Vec2(0.0f,1.0f);

    triangles[0] = 0;
    triangles[1] = 1;
    triangles[2] = 2;
    triangles[3] = 0;
    triangles[4] = 2;
    triangles[5] = 3;

    Mesh* mesh = new Mesh;
    mesh->cold->vertices = 4;
    mesh->cold->normals = 4;
    mesh->faces = 2;
    mesh->cold->data = (void*)data;
    return mesh;
}

#endif // PRIMITIVES_H
