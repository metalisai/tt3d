#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "mesh.h"
#include "ttMath.h"

// ignores until delimiter
// returns false if end of file was reached
bool ignore(FILE* file, char delim)
{
    char ch = fgetc(file);
    while(ch != delim && ch != EOF)
    {
        ch = fgetc(file);
    }
    return (ch != EOF);
}

void skip(FILE* file, char delim)
{
    char ch = fgetc(file);
    while(ch == delim && ch != EOF)
    {
        ch = fgetc(file);
    }
    ungetc(ch,file);
}

// TODO: can overflow
int readToken(FILE* file, char* buffer, int size)
{
    char ch = fgetc(file);
    int charIndex = 0;
    if(ch == EOF)
        printf("END OF FILE! \n");
    while(ch != ' ' && ch != '\n' && ch != EOF && charIndex < size)
    {
        buffer[charIndex] = ch;
        charIndex++;
        ch = fgetc(file);
    }
    ungetc(ch, file);
    buffer[charIndex] = '\0';
    return charIndex;
}

char readc(FILE* file)
{
    long offset = ftell(file);
    char c1 = fgetc(file);
    fseek ( file , offset , SEEK_SET );
    return c1;
}

void getModelStats(char const* fileName, int& verts, int& faces/*, int& normals*/)
{
    FILE* file = fopen(fileName, "r");
    if(file == NULL)
    {
        printf("File %s not found! \n", fileName);
        return;
    }

    verts= 0;
    faces= 0;
    int texcoords = 0;

    char ch;
    while( ( ch = fgetc(file) ) != EOF )
    {
        int floatIndex = 0;
        switch(ch)
        {
            case '#':
                ignore(file,'\n');
                break;
            case 'v':
                ch = fgetc(file);
                if(ch == ' ')
                {
                    ignore(file,'\n');
                    verts++;
                }
                else if(ch == 'n')
                {
                    ignore(file,'\n');
                    //normals++;
                }
                else if(ch == 't')
                {
                    ignore(file,'\n');
                    texcoords++;
                }
                break;
            case 'f':
                ch = fgetc(file);
                if(ch == ' ')
                {
                    while(ch != '\n' &&
                          ch != EOF)
                    {
                        char index[50];
                        skip(file,' ');
                        readToken(file,index,50); // TODO: no need to use the data, so...
                        floatIndex++;
                        ch = readc(file);
                    }
                    assert(floatIndex == 3 || floatIndex == 4);
                    faces+=floatIndex-2;
                    ch = fgetc(file);
                }
                break;
            default:
                ignore(file,'\n');
                break;
        }
    }
    printf("tc %d\n",texcoords);
    fclose(file);
}

Mesh* loadObjMesh(const char* fileName)
{
    printf("starting mesh loading\n");
    FILE* file = fopen(fileName, "r");
    if(file == NULL)
    {
        printf("File %s not found!\n", fileName);
        return NULL;
    }

    char ch;

    int verts = 0;
    int faces = 0;
    int normals = 0;

    getModelStats(fileName,verts,faces/*,normals*/);
    printf("%d", verts);
    normals = verts;

    const void* modelData = malloc(verts*sizeof(Vec4)+verts*sizeof(Vec3)+verts*sizeof(Vec2)+faces*3*sizeof(u16));
    const Vec2* texCoordBuffer = (Vec2*)malloc(verts*sizeof(Vec2));
    char* dataCursor = (char*)modelData;

    int mode = -1;
    while( ( ch = fgetc(file) ) != EOF )
    {
        int floatIndex = 0;

        int elCounter = 0;

        switch(ch)
        {
            case '#':
                ignore(file,'\n');
                break;
            case 'v':
                switch(fgetc(file))
                {
                case ' ':
                    if(mode != 0)
                    {
                        mode = 0;
                        dataCursor = (char*)modelData;
                        elCounter = 0;
                        //printf("started reading vertices\n");
                    }
                    else
                    {
                        elCounter++;
                    }
                    while(ch != '\n' &&
                          ch != EOF)
                    {
                        char vert[50];
                        skip(file,' ');
                        readToken(file,vert,50);

                        *(r32*)dataCursor = strtof(vert,NULL);
                        dataCursor += 4;

                        floatIndex++;
                        ch = readc(file);
                    }
                    ch = fgetc(file);
                    while(floatIndex < 4)
                    {
                        *(r32*)dataCursor = 1.0f;
                        dataCursor += 4;
                        floatIndex++;
                    }
                    break;
                case 'n':
                    if(mode != 1)
                    {
                        mode = 1;
                        dataCursor = (char*)modelData+verts*sizeof(Vec4);
                        elCounter = 0;
                        //printf("started reading normals/n");
                    }
                    else
                    {
                        elCounter++;
                    }
                    //ignore(file,'\n');
                    floatIndex = 0;
                    //float norms[3];
                    while(ch != '\n' &&
                          ch != EOF)
                    {
                        char vert[50];
                        skip(file,' ');
                        readToken(file,vert,50);

                        //*(float*)dataCursor = strtof(vert,NULL);
                        *(r32*)dataCursor = 0.0f;
                        dataCursor += 4;
                        //norms[floatIndex] = strtof(vert,NULL);
                        floatIndex++;
                        ch = readc(file);
                    }
                    assert(floatIndex == 3);

                    //printf("normal %f %f %f\n",norms[0],norms[1],norms[2]);

                    ch = fgetc(file);
                    break;

                case 't':
                    if(mode != 3)
                    {
                        mode = 3;
                        dataCursor = (char*)texCoordBuffer;
                        elCounter = 0;
                        //printf("started reading normals/n");
                    }
                    else
                    {
                        elCounter++;
                    }
                    floatIndex = 0;
                    while(ch != '\n' &&
                          ch != EOF)
                    {
                        char vert[50];
                        skip(file,' ');
                        readToken(file,vert,50);

                        //*(r32*)dataCursor = 0.0f;
                        //dataCursor += sizeof(r32);
                        floatIndex++;
                        ch = readc(file);
                    }
                    assert(floatIndex == 2);

                    ch = fgetc(file);
                    break;
                }
                break;
            case 'f':
                if(fgetc(file) == ' ')
                {
                    if(mode != 2)
                    {
                        mode = 2;
                        dataCursor = (char*)modelData+verts*sizeof(Vec4)+verts*sizeof(Vec3)+verts*sizeof(Vec2);
                        elCounter = 0;
                        //printf("started reading faces\n");
                    }

                    int floatIndex = 0;
                    u16 indices[4];
                    //u16 texIndices[4];

                    while(ch != '\n' &&
                          ch != EOF)
                    {
                        char index[50];
                        skip(file,' ');
                        readToken(file,index,50);

                        indices[floatIndex] = (u16)(atoi(index)-1); // we need 0 based indices

                        int slashes = 0;
                        //int locations[2];
                        for(u32 i=0;i < sizeof(index);++i)
                        {
                            if(index[i] == '/')
                            {
                                //locations[slashes] = i;
                                slashes++;
                                if(slashes >= 2)
                                    break;
                            }
                        }

                        /*if(slashes > 0)
                            texIndices[floatIndex] = (u16)(atoi(&index[locations[0]]));*/


                        floatIndex++;
                        ch = readc(file);
                    }

                    switch(floatIndex)
                    {
                    case 3:
                        //printf("face poly %hu %hu %hu\n",indices[0],indices[1],indices[2]);
                        memcpy(dataCursor,indices,sizeof(u16)*3);
                        dataCursor += 6;
                        elCounter++;
                        break;
                    case 4:
                        //printf("face poly %hu %hu %hu %hu\n",indices[0],indices[1],indices[2], indices[3]);
                        memcpy(dataCursor,indices,sizeof(u16)*3);
                        memcpy(dataCursor+6,indices,sizeof(u16));
                        memcpy(dataCursor+8,&indices[2],sizeof(u16)*2);
                        dataCursor += 12;
                        elCounter+=2;
                        break;
                    default:
                        assert(false); // invalid number of facesS
                    }

                    ch = fgetc(file);
                }
                break;
            case ' ':
                break;
            default:
                ch = fgetc(file);
                while(ch != '\n' && ch != EOF)
                {
                    ch = fgetc(file);
                }
                break;
        }
    }

#if 1
    char* pp = (char*)modelData;
    for(int i = 0; i < verts; i++)
    {
        printf("vert2 %f %f %f %f\n",((r32*)pp)[0],((r32*)pp)[1],((r32*)pp)[2],((r32*)pp)[3]);
        pp+=16;
    }
    for(int i = 0; i < verts; i++)
    {
        printf("norm2 %f %f %f\n",((r32*)pp)[0],((r32*)pp)[1],((r32*)pp)[2]);
        pp+=12;
    }
    for(int i = 0; i < verts; i++)
    {
        printf("tex2 %f %f\n",((r32*)pp)[0],((r32*)pp)[1]);
        pp+=8;
    }
    for(int i = 0; i < faces; i++)
    {
        printf("face2 %hu %hu %hu\n",((u16*)pp)[0],((u16*)pp)[1],((u16*)pp)[2]);
        pp+=6;
    }
#endif

    printf("Model loaded verts: %d faces %d normals %d\n",verts,faces, normals);
    fclose(file);

    Mesh* ret = new Mesh;
    ret->cold->vertices = verts;
    ret->faces = faces;
    ret->cold->normals = normals;
    ret->cold->data = (void*)modelData;

    //free((void*)texCoordBuffer);

    return ret;
}

Mesh* loadMesh(const char* fileName)
{
    printf("starting mesh loading\n");
    FILE* file = fopen(fileName, "r");
    if(file == NULL)
    {
        printf("File %s not found!\n", fileName);
        return NULL;
    }
    u16 verts;
    u16 faces;

    fread(&verts,sizeof(u16),1,file);
    fread(&faces,sizeof(u16),1,file);
    printf("verts: %hu, faces: %hu\n",verts,faces);

    int byteCount = verts*sizeof(Vec4)+verts*sizeof(Vec3)+verts*sizeof(Vec2)+faces*3*sizeof(u16)+verts*sizeof(Vec3);
    void* data = malloc(byteCount);
    fread(data, 1, byteCount, file);

    Mesh* mesh = new Mesh();
    mesh->cold->data = data;
    mesh->cold->vertices = verts;
    mesh->faces = faces;

    meshRecalculateBounds(mesh);

#if 0
    char* pp = (char*)mesh->data;
    for(int i = 0; i < verts; i++)
    {
        printf("%d vert2 %f %f %f %f\n",i,((r32*)pp)[0],((r32*)pp)[1],((r32*)pp)[2],((r32*)pp)[3]);
        pp+=16;
    }
    for(int i = 0; i < verts; i++)
    {
        printf("norm2 %f %f %f\n",((r32*)pp)[0],((r32*)pp)[1],((r32*)pp)[2]);
        pp+=12;
    }
    for(int i = 0; i < verts; i++)
    {
        printf("tex2 %f %f\n",((r32*)pp)[0],((r32*)pp)[1]);
        pp+=8;
    }
    for(int i = 0; i < faces; i++)
    {
        printf("face2 %hu %hu %hu\n",((u16*)pp)[0],((u16*)pp)[1],((u16*)pp)[2]);
        pp+=6;
    }
#endif

    return mesh;
}

/*int main()
{
    Mesh* mesh = loadMesh("cube.obj");
    free(mesh);
    return 0;
}*/
