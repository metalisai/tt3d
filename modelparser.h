#ifndef MODELPARSER_H
#define MODELPARSER_H

#include "mesh.h"

bool ignore(FILE* file, char delim);
void skip(FILE* file, char delim);
int readToken(FILE* file, char* buffer, int size);
char readc(FILE* file);
void getModelStats(char const* fileName, int& verts, int& faces/*, int& normals*/);
Mesh* loadObjMesh(const char* fileName);
Mesh* loadMesh(const char* fileName);


#endif // MODELPARSER_H
