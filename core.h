#ifndef CORE_H
#define CORE_H

#include <GL/glew.h>

#include "engine_platform.h"
#include "engine.h"

typedef struct ForwardPlus
{
    u32 workGroupsX;
    u32 workGroupsY;
    GLuint lightBuffer;
    GLuint visibleLightIndicesBuffer;
} ForwardPlus;

typedef struct Particle
{
    Vec3 localPosition;
    r32 timer;
    r32 fade;
} Particles;

typedef struct ParticleVAOData
{
    Vec3 position;
    r32 fade;
} ParticleVAOData;

typedef struct ParticleSystem
{
    LoadedTexture texture;

    Vec3 position;
    Vec3 velocity;
    r32 lifeTime;
    i32 maxParticles;
    i32 lastDestroyedIndex;
    i32 lastSpawnedIndex;
    r32 timeSinceLastParticle;

    GLuint GLBuffer;
    GLuint VAO;

    struct Particle particles[1000];
    ParticleVAOData particleData[1000];
} ParticleSystem;

typedef struct Game_State
{
    Vec4 sunDir;
    Mat4 sunRot2;// TODO: remove
    Quaternion sunRot2Quat;

    Shader texShaderForw;
    Shader texShader;
    Shader notexShader;
    Shader skyShader;
    Shader normalShader;
    Shader lineShader;
    Shader particleShader;

    Shader straightShader;

    Entity barrel[3600];
    Entity terrain;
    Entity testBarrel;
    Entity wall;
    Entity wall2;
    Entity voxelTerrain[100];
    Entity earth;
    Entity dome;

    Material barrelMat;
    Material grassMat;

    ParticleSystem particles;
} Game_State;

typedef struct Permanent_Storage{
    Camera main_cam;
    TexturePool* texturePool;
    i32 windowWidth;
    i32 windowHeight;
    int numEntities;
    Entity* entities[5000];
    Vec3 minimumPositions;
    Vec3 maximumPositions;

    /*GLuint fbo;
    GLuint fbo_texture;
    GLuint fbo_normal;
    GLuint rbo_depth;
    GLuint fbo_position;*/
    GLuint vbo_fbo_vertices;
    OpenglFrameBuffer defferedFbo;

    //GLuint shadow_fbo;
    //GLuint shadow_fbo_depth;
    OpenglFrameBuffer shadowmap_fbo;
    OpenglFrameBuffer zprepass_fbo;

    /*GLuint zprepass_fbo;
    GLuint zprepass_fbo_depth;*/
    ForwardPlus fplus;

    Shader postProcShader;
    Shader postProcPointShader;
    Shader shadowPassShader;
    Shader lightCullShader;
    Shader textShader;

    b32 deferred;

    // TODO: make a shader pool
    Shader* shaders[10];
    u32 numShaders;

    u32 mcubesBuffer;
    u32 mcubesTexture;
    u32 mcubesTexture2;
    u32 tfeedbackBuffer;
    u32 tfeedback;
    u32 numberOfTriangles;
    b32 captured;

    Game_State game;
} Permanent_Storage;

typedef struct PointLight {
    Vec4 color;
    Vec4 position;
    Vec4 paddingAndRadius;
} PointLight;

inline void addEntity(Permanent_Storage* state, Entity* ent);
static inline void addSurfaceShader(Permanent_Storage* state, Shader* shader);

#ifdef __cplusplus
extern "C" {
#endif

void init(EngineMemory* gamemem, int width, int height);
void display(EngineMemory* gamemem, Input* input, float dt);
void keyboard( unsigned char key, int mouseX, int mouseY );
void pMouse(int x, int y);
void reshape (EngineMemory* game_memory, int w, int h);
void gameExit(EngineMemory* game_memory);

#ifdef __cplusplus
}
#endif

#endif // CORE_H
