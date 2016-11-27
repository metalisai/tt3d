#ifndef ENGINE_H
#define ENGINE_H

#include <stdio.h>
#include <GL/glew.h>

#include "shared.h"
#include "renderer.h"


typedef struct Character {
    GLuint     TextureID;  // ID handle of the glyph texture
    IVec2 Size;       // Size of glyph
    IVec2 Bearing;    // Offset from baseline to left/top of glyph
    GLuint     Advance;    // Offset to advance to next glyph
} Character;

void InitText(); // in debug.cpp
void RenderText(char* text, GLfloat x, GLfloat y, GLfloat scale);


#define KEYCODE_Q               1
#define KEYCODE_W               2
#define KEYCODE_E               3
#define KEYCODE_R               4
#define KEYCODE_T               5
#define KEYCODE_Y               6
#define KEYCODE_U               7
#define KEYCODE_I               8
#define KEYCODE_O               9
#define KEYCODE_P               10
#define KEYCODE_A               11
#define KEYCODE_S               12
#define KEYCODE_D               13
#define KEYCODE_F               14
#define KEYCODE_G               15
#define KEYCODE_H               16
#define KEYCODE_J               17
#define KEYCODE_K               18
#define KEYCODE_L               19
#define KEYCODE_Z               20
#define KEYCODE_X               21
#define KEYCODE_C               22
#define KEYCODE_V               23
#define KEYCODE_B               24
#define KEYCODE_N               25
#define KEYCODE_M               26
#define KEYCODE_TAB             27
#define KEYCODE_CAPSLOCK        28
#define KEYCODE_LSHIFT          29
#define KEYCODE_LCTRL           30

#define getKeyDown(input, keycode) \
    ((input)->keyStates[(keycode)] == 1 && (input)->keyStatesLastFrame[(keycode)] == 0)

#define getKeyUp(input, keycode) \
    ((input)->keyStates[(keycode)] == 1 && (input)->keyStatesLastFrame[(keycode)] == 0)

#define getKey(input, keycode) \
    ((input)->keyStates[(keycode)])

Mesh *generatePlane();

#define NUM_BUFFERS 2

int initAudio();
void setListenerTransform(Vec3 pos, Vec3 fwd, Vec3 up);
void audioExit();

void setupDebug();
void drawLine(Vec3 start, Vec3 end);
void drawDebugLines(Shader lineShader, Camera *cam);

b32 ignore(FILE *file, char delim);
void skip(FILE *file, char delim);
int readToken(FILE *file, char* buffer, int size);
char readc(FILE *file);
void getModelStats(char const* fileName, int* verts, int* faces/*, int& normals*/);
Mesh *loadObjMesh(const char* fileName);
Mesh *loadMesh(const char* fileName);

Mesh *generateTerrainMesh();

#endif // ENGINE_H
