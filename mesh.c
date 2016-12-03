#include "renderer.h"

#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <math.h>

void meshInit(Mesh* mesh)
{
    mesh->VAO = -1;
    mesh->AttribBuffer = -1;
    mesh->ElementBuffer = -1;
    mesh->cold = malloc(sizeof(MeshCold)); // TODO: this never gets deleted?
    mesh->cold->loadedToGPU = false;
    mesh->cold->data = NULL;
}

void openGL_loadMesh(Mesh* mesh)
{
    // generate and populate buffers
    assert(!mesh->cold->loadedToGPU);
    int vertices = mesh->cold->vertices;
    void* data = mesh->cold->data;

    glGenBuffers(1, &mesh->AttribBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, mesh->AttribBuffer);
    glBufferData(GL_ARRAY_BUFFER, vertices*sizeof(Vec4)+vertices*sizeof(Vec3)+vertices*sizeof(Vec2)+vertices*sizeof(Vec3), data, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, vertices*sizeof(Vec4)+vertices*sizeof(Vec3)+vertices*sizeof(Vec2),
                    vertices*sizeof(Vec3),
                    ((char*)data)+vertices*sizeof(Vec4)+vertices*sizeof(Vec3)+vertices*sizeof(Vec2)+mesh->faces*3*sizeof(u16));
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glGenBuffers(1, &mesh->ElementBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->ElementBuffer);
    u32 byteOffset = vertices*sizeof(Vec4)+vertices*sizeof(Vec3)+vertices*sizeof(Vec2);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh->faces*3*sizeof(u16), ((char*)data)+byteOffset, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    glGenVertexArrays(1, &mesh->VAO);
    glBindVertexArray(mesh->VAO);
    glBindBuffer(GL_ARRAY_BUFFER, mesh->AttribBuffer);
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    glEnableVertexAttribArray(3);
    // positions
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);
    // normals
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)(vertices*sizeof(Vec4)));
    // texcoord
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (void*)(vertices*sizeof(Vec4)+vertices*sizeof(Vec3)));
    // tangents
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0, (void*)(vertices*sizeof(Vec4)+vertices*sizeof(Vec3)+vertices*sizeof(Vec2)));
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->ElementBuffer);
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    mesh->cold->loadedToGPU = true;
}

void renderMesh(Mesh* mesh)
{
    glBindVertexArray(mesh->VAO);
    glEnableVertexAttribArray(0); // pos
    glEnableVertexAttribArray(1); // normal
    glEnableVertexAttribArray(2); // tex
    glEnableVertexAttribArray(3); // tangent
    glDrawElements(GL_TRIANGLES, mesh->faces*3, GL_UNSIGNED_SHORT, 0);
    glDisableVertexAttribArray(0); // pos
    glDisableVertexAttribArray(1); // normal
    glDisableVertexAttribArray(2); // tex
    glDisableVertexAttribArray(3); // tangent
    glBindVertexArray(0);
}

void meshRecalculateBounds(Mesh* mesh)
{
    r32 max = -999999.0;
    Vec4* verts = (Vec4*)mesh->cold->data;
    for(int i = 0; i < mesh->cold->vertices; i++)
    {
        r32 length2 = verts[i].x*verts[i].x+verts[i].y*verts[i].y+verts[i].z*verts[i].z;
        if(length2 > max)
            max = length2;
    }
    assert(max > 0);
    mesh->boundingRadius = sqrt(max);
}

void meshRecalculateNormals(Mesh* mesh)
{
    void* modelData = mesh->cold->data;
    int verts = mesh->cold->vertices;
    int faces = mesh->faces;
    Vec4* vertsP = (Vec4*)((char*)modelData);
    Vec3* normalsP = (Vec3*)(((char*)modelData)+verts*sizeof(Vec4));
    Vec2* texP = (Vec2*)(normalsP+verts);
    u16* facesP = (u16*)(texP+verts);
    Vec3* tangP = (Vec3*)(facesP+faces*3);
    memset(normalsP,0,sizeof(Vec3)*verts);
    //printf("verts %d\n",verts);
    for(int i = 0; i < faces; i++)
    {
        //printf("index: %d ",facesP[i*3]);
        Vec3 a = vec3(vertsP[facesP[i*3+0]].x,vertsP[facesP[i*3+0]].y,vertsP[facesP[i*3+0]].z);
        Vec3 b = vec3(vertsP[facesP[i*3+1]].x,vertsP[facesP[i*3+1]].y,vertsP[facesP[i*3+1]].z);
        Vec3 c = vec3(vertsP[facesP[i*3+2]].x,vertsP[facesP[i*3+2]].y,vertsP[facesP[i*3+2]].z);

        Vec2 texA = vec2(texP[facesP[i*3+0]].x,texP[facesP[i*3+0]].y);
        Vec2 texB = vec2(texP[facesP[i*3+1]].x,texP[facesP[i*3+1]].y);
        Vec2 texC = vec2(texP[facesP[i*3+2]].x,texP[facesP[i*3+2]].y);

        Vec3 edge1;
        Vec3 edge2;
        vec3Sub(&edge1, &c, &a);
        vec3Sub(&edge2, &b, &c);

        Vec2 texEdge1;
        Vec2 texEdge2;
        vec2Sub(&texEdge1, &texC, &texA);
        vec2Sub(&texEdge2, &texB, &texC);

        Vec3 normal = vec3Cross(&edge2,&edge1);
        if(vec3Mag(&normal) > 0.0001)
        {
            normal = vec3Normalized(&normal);
        }
        normalsP[facesP[i*3+0]].x += normal.x;
        normalsP[facesP[i*3+0]].y += normal.y;
        normalsP[facesP[i*3+0]].z += normal.z;

        normalsP[facesP[i*3+1]].x += normal.x;
        normalsP[facesP[i*3+1]].y += normal.y;
        normalsP[facesP[i*3+1]].z += normal.z;

        normalsP[facesP[i*3+2]].x += normal.x;
        normalsP[facesP[i*3+2]].y += normal.y;
        normalsP[facesP[i*3+2]].z += normal.z;

        float coef = 1/ (texEdge1.x * texEdge2.y - texEdge2.x * texEdge1.y);
        Vec3 tangent;
        tangent.x = coef * ((edge1.x * texEdge2.y)  + (edge2.x * -texEdge1.y));
        tangent.y = coef * ((edge1.y * texEdge2.y)  + (edge2.y * -texEdge1.y));
        tangent.z = coef * ((edge1.z * texEdge2.y)  + (edge2.z * -texEdge1.y));
        //float3 binormal = normal.crossProduct(tangent);
        tangP[facesP[i*3+0]] = tangent;
        tangP[facesP[i*3+1]] = tangent;
        tangP[facesP[i*3+2]] = tangent;

        if(isnan(normal.x) || isnan(normal.y) || isnan(normal.z))
        {
            //printf("Nan while calculating normals (%d %f,%f,%f)(%d %f,%f,%f)(%d %f,%f,%f)",facesP[i*3+0],a.x,a.y,a.z,facesP[i*3+1],b.x,b.y,b.z,facesP[i*3+2],c.x,c.y,c.z);
            normal.x = 1.0f;
            normal.y = 0.0f;
            normal.z = 0.0f;
            // completely random, if this happens, it means that the triangle had equal vertices, thus it won't even be visible
            // so we just use a random vector so the math doesn't blow up
        }
        //printf("norm %f %f %f\n",normal.x,normal.y,normal.z);
    }
    //printf("\n");

   for(int i = 0; i < verts; i++)
    {
        Vec3 a = vec3(normalsP[i].x,normalsP[i].y,normalsP[i].z);

        if(vec3Mag(&a) > 0.0001)
        {
            Vec3 normed = vec3Normalized(&a);
            normalsP[i].x = normed.x;
            normalsP[i].y = normed.y;
            normalsP[i].z = normed.z;

            if(isnan(normed.x) || isnan(normed.y) || isnan(normed.z))
            {
                //printf("Nan while calculating normals2\n");
            }
        }
        else
        {
            //printf("Normal vector was 0 lenght!\n");
        }
    }
}

Mesh* generatePlane()
{
    int vertexSize = 4*sizeof(Vec4);
    int normalSize = 4*sizeof(Vec3);
    int texSize = 4*sizeof(Vec2);
    int indexSize = (6 * sizeof(u16));
    int tangentSize = 4*sizeof(Vec3);
    const void* data = malloc(vertexSize+normalSize+indexSize+texSize+tangentSize);

    Vec4* vertices = (Vec4*)data;

    char* data2 = (char*)data;
    Vec3* normals = (Vec3*)((data2+4*sizeof(Vec4)));
    Vec2* uv = (Vec2*)(normals + 4);
    data2 += 4*sizeof(Vec4)+4*sizeof(Vec3)+4*sizeof(Vec2);
    u16* triangles = (u16*)data2;

    vertices[0] =  vec4(-0.5f,-0.5f,-0.0f,1.0f);
    vertices[1] =  vec4(0.5f,-0.5f,-0.0f ,1.0f);
    vertices[2] =  vec4(0.5f,0.5f,0.0f  ,1.0f);
    vertices[3] =  vec4(-0.5f,0.5f,0.0f ,1.0f);

    uv[0] = vec2(0.0f,0.0f);
    uv[1] = vec2(1.0f,0.0f);
    uv[2] = vec2(1.0f,1.0f);
    uv[3] = vec2(0.0f,1.0f);

    triangles[0] = 0;
    triangles[1] = 1;
    triangles[2] = 2;
    triangles[3] = 0;
    triangles[4] = 2;
    triangles[5] = 3;

    Mesh* mesh = malloc(sizeof(Mesh));
    meshInit(mesh);
    mesh->cold->vertices = 4;
    mesh->cold->normals = 4;
    mesh->faces = 2;
    mesh->cold->data = (void*)data;
    return mesh;
}
