#ifndef ENGINE_H
#define ENGINE_H

#include <GL/glew.h>

#include "shared.h"
#include "renderer.h"


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
#define MAX_TUNNELS 64

typedef struct DebugState
{
    void* lineMem;
    u32 lines;
    GLuint lineBuffer;
    GLuint lineVAO;
} DebugState;

typedef struct Line3D
{
    Vec4 start;
    Vec4 end;
} Line3D;

typedef struct ChunkGenData
{
    u32 tunnelCount;
    r32 firstOctaveMax;
    r32 secondOctaveMax;
    r32 padding;
    r32 dxgoalFirstOctaveMax;
    r32 dxgoalSecondOctaveMax;
    r32 dzgoalFirstOctaveMax;
    r32 dzgoalSecondOctaveMax;
    Vec4 chunkOrigin;
    Line3D tunnels[MAX_TUNNELS];
} ChunkGenData;

typedef struct TerrainGeneratorState
{
    GLuint vertInbuffer;
    ChunkGenData tunnelData;
    GLuint vertAtomicBuffer;
    GLuint tunnelBuffer;
    GLuint edgeVertexBuffer;
    u32 edgeVertexBufferSize;
    r32 voxelScale;
    b32 initialized;
} TerrainGeneratorState;

int initAudio();
void setListenerTransform(Vec3 pos, Vec3 fwd, Vec3 up);
void audioExit();

void setupDebug(DebugState* dstate);
void drawLine(DebugState* dstate, Vec3 start, Vec3 end);
void drawDebugLines(DebugState* dstate, Shader lineShader, Camera *cam);

b32 ignore(FILE *file, char delim);
void skip(FILE *file, char delim);
int readToken(FILE *file, char* buffer, int size);
char readc(FILE *file);
void getModelStats(char const* fileName, int* verts, int* faces/*, int& normals*/);
Mesh *loadObjMesh(const char* fileName);
Mesh *loadMesh(const char* fileName);

Mesh *generateTerrainMesh();
Mesh *terrainGen(r32 y);

void openglInitializeTerrainGeneration(TerrainGeneratorState* tgstate, u32 maxGroups, u32 cubesPerSeed, r32 voxelScale);
void openglPrepageTerrainGeneration(TerrainGeneratorState* tgstate, GLuint outBuffer, GLuint outElementBuffer, u32 groups, r32 scale);

#endif // ENGINE_H
