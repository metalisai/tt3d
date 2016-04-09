#ifndef MESH_H
#define MESH_H

//#include <GL/gl.h>
#include <GL/glew.h>
#include "ttMath.h"
#include "types.h"

// data that doesn't need to be accessed on a regular basis
struct MeshCold
{
    int vertices;
    int normals;
    bool loadedToGPU = false;
    void* data = NULL;
};

struct Mesh
{
    int faces;
    GLuint VAO = -1;
    GLuint AttribBuffer = -1;
    GLuint ElementBuffer = -1;
    MeshCold* cold;

    r32 boundingRadius;

    Mesh();
    //~Mesh();
};

void openGL_loadMesh(Mesh* mesh);
void renderMesh(Mesh* mesh);
void meshRecalculateNormals(Mesh* mesh);
void meshRecalculateBounds(Mesh* mesh);

#endif // MESH_H
