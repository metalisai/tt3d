#ifndef CORE_H
#define CORE_H

#include <GL/glew.h>

#include "engine_platform.h"
#include "engine.h"

struct ForwardPlus
{
    int workGroupsX;
    int workGroupsY;
    GLuint lightBuffer;
    GLuint visibleLightIndicesBuffer;
};

struct Particle
{
    Vec3 localPosition;
    r32 timer;
    r32 fade;
};

struct ParticleVAOData
{
    Vec3 position;
    r32 fade;
};

struct ParticleSystem
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

    Particle particles[1000];
    ParticleVAOData particleData[1000];
};

struct Game_State
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

    Entity barrel[3600];
    Entity terrain;
    Entity testBarrel;
    Entity wall;
    Entity wall2;
    Entity earth;
    Entity dome;

    Material barrelMat;
    Material grassMat;

    ParticleSystem particles;
};

struct Permanent_Storage{
    Camera main_cam;
    TexturePool* texturePool;
    i32 windowWidth;
    i32 windowHeight;
    int numEntities;
    Entity* entities[5000];

    GLuint fbo;
    GLuint fbo_texture;
    GLuint fbo_normal;
    GLuint rbo_depth;
    GLuint fbo_position;
    GLuint vbo_fbo_vertices;

    GLuint shadow_fbo;
    GLuint shadow_fbo_depth;

    GLuint zprepass_fbo;
    GLuint zprepass_fbo_depth;
    ForwardPlus fplus;

    Shader postProcShader;
    Shader postProcPointShader;
    Shader shadowPassShader;
    Shader lightCullShader;
    Shader textShader;

    bool deferred = true;

    // TODO: make a shader pool
    Shader* shaders[10];
    u32 numShaders;

    Game_State game;
};

inline void addEntity(Permanent_Storage* state, Entity* ent);
static inline void addSurfaceShader(Permanent_Storage* state, Shader* shader);
extern "C"
{
    void init(EngineMemory* gamemem, int width, int height);
    void display(EngineMemory* gamemem, Input* input, float dt);
    void keyboard( unsigned char key, int mouseX, int mouseY );
    void pMouse(int x, int y);
    void reshape (EngineMemory* game_memory, int w, int h);
    void gameExit(EngineMemory* game_memory);
}

#endif // CORE_H
