#ifndef CORE_H
#define CORE_H

#include <GL/glew.h>

#include "texture.h"
#include "camera.h"
#include "entity.h"
#include "shader.h"
#include "memory.h"
#include "input.h"

struct ForwardPlus
{
    int workGroupsX;
    int workGroupsY;
    GLuint lightBuffer;
    GLuint visibleLightIndicesBuffer;
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

    Entity barrel[3600];
    Entity terrain;
    Entity testBarrel;
    Entity wall;
    Entity wall2;
    Entity earth;
    Entity dome;

    Material barrelMat;
    Material grassMat;
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
    void init(MemStack* gamemem, int width, int height);
    void display(MemStack* gamemem, Input* input, float dt);
    void keyboard( unsigned char key, int mouseX, int mouseY );
    void pMouse(int x, int y);
    void reshape (MemStack* game_memory, int w, int h);
    void gameExit(MemStack* game_memory);
}

#endif // CORE_H
