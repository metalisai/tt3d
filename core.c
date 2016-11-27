#include "core.h"

#include <stdio.h>
#include <math.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>

#define STB_IMAGE_IMPLEMENTATION
//#include "stb_image.h"

#define ARRAY_COUNT( array ) (sizeof( array ) / (sizeof( array[0] )))

Mesh *domeMesh, *mesh, *terrainMesh;


//Mesh *terrainMesh[10][10]/*= generateJunk(0,0,0)*/;

#define SHADOWMAP_RES   2048

void addEntity(Permanent_Storage *state, Entity *ent)
{
    state->entities[state->numEntities] = ent;
    ++state->numEntities;
}

static void addSurfaceShader(Permanent_Storage *state, Shader *shader)
{
    state->shaders[state->numShaders] = shader;
    ++state->numShaders;
}

void deleteFrameBuffer(Permanent_Storage *gstate)
{
    glDeleteFramebuffers(1, &gstate->fbo);
    glDeleteTextures(1, &gstate->fbo_texture);
    glDeleteTextures(1, &gstate->fbo_normal);
    glDeleteTextures(1, &gstate->fbo_position);
    glDeleteTextures(1, &gstate->rbo_depth);
}

void deleteShadowBuffer(Permanent_Storage *gstate)
{
    glDeleteFramebuffers(1, &gstate->shadow_fbo);
    glDeleteTextures(1, &gstate->shadow_fbo_depth);
}

void deleteZPassBuffer(Permanent_Storage *gstate)
{
    glDeleteFramebuffers(1, &gstate->zprepass_fbo);
    glDeleteTextures(1, &gstate->zprepass_fbo_depth);
}

// TODO: make compressed version to create a depth buffer fbo
void genZPassBuffer(Permanent_Storage *gstate, int width, int height)
{
    glActiveTexture(GL_TEXTURE0);
    GLfloat cols[4];
    cols[0] = 1.0;
    cols[1] = 1.0;
    cols[2] = 1.0;
    cols[3] = 1.0;

    glGenTextures(1, &gstate->zprepass_fbo_depth);
    glBindTexture(GL_TEXTURE_2D, gstate->zprepass_fbo_depth);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, NULL);
    glBindTexture(GL_TEXTURE_2D, 0);


    glGenFramebuffers(1, &gstate->zprepass_fbo);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, gstate->zprepass_fbo);
    glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, gstate->zprepass_fbo_depth, 0);

    GLenum status;
    if ((status = glCheckFramebufferStatus(GL_FRAMEBUFFER)) != GL_FRAMEBUFFER_COMPLETE) {
      fprintf(stderr, "glCheckFramebufferStatus: error %d", status);
      return;
    }
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
}

#define FPLUS_TILESIZE 16
#define NUM_LIGHTS 1024

void genForwardPlusBuffers(Permanent_Storage *state, int width, int height)
{
    // TODO: this
    typedef int VisibleIndex;

    state->fplus.workGroupsX = (width + (width % FPLUS_TILESIZE)) / FPLUS_TILESIZE;
    state->fplus.workGroupsY = (height + (height % FPLUS_TILESIZE)) / FPLUS_TILESIZE;
    size_t numberOfTiles = state->fplus.workGroupsX  *state->fplus.workGroupsY;
    printf("tiles %d tilesX %d\n",numberOfTiles,state->fplus.workGroupsX);

    // Generate our shader storage buffers
    glGenBuffers(1, &state->fplus.lightBuffer);
    glGenBuffers(1, &state->fplus.visibleLightIndicesBuffer);

    // Bind light buffer
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, state->fplus.lightBuffer);
    glBufferData(GL_SHADER_STORAGE_BUFFER, NUM_LIGHTS * sizeof(PointLight), 0, GL_DYNAMIC_DRAW);

    // Bind visible light indices buffer
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, state->fplus.visibleLightIndicesBuffer);
    glBufferData(GL_SHADER_STORAGE_BUFFER, numberOfTiles * sizeof(VisibleIndex) * 1024, 0, GL_STATIC_DRAW);

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void genShadowBuffer(Permanent_Storage *gstate, int width, int height)
{
    glActiveTexture(GL_TEXTURE0);
    GLfloat cols[4];
    cols[0] = 1.0;
    cols[1] = 1.0;
    cols[2] = 1.0;
    cols[3] = 1.0;

    glGenTextures(1, &gstate->shadow_fbo_depth);
    glBindTexture(GL_TEXTURE_2D, gstate->shadow_fbo_depth);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, cols);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, NULL);
    glBindTexture(GL_TEXTURE_2D, 0);


    glGenFramebuffers(1, &gstate->shadow_fbo);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, gstate->shadow_fbo);
    glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, gstate->shadow_fbo_depth, 0);

    GLenum status;
    if ((status = glCheckFramebufferStatus(GL_FRAMEBUFFER)) != GL_FRAMEBUFFER_COMPLETE) {
      fprintf(stderr, "glCheckFramebufferStatus: error %d", status);
      return;
    }
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
}

void genFrameBuffer(Permanent_Storage *gstate, int screenW, int screenH)
{
    glActiveTexture(GL_TEXTURE0);
    glGenTextures(1, &gstate->fbo_texture);
    glBindTexture(GL_TEXTURE_2D, gstate->fbo_texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, screenW, screenH, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glBindTexture(GL_TEXTURE_2D, 0);

    glGenTextures(1, &gstate->fbo_normal);
    glBindTexture(GL_TEXTURE_2D, gstate->fbo_normal);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, screenW, screenH, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glBindTexture(GL_TEXTURE_2D, 0);

    glGenTextures(1, &gstate->fbo_position);
    glBindTexture(GL_TEXTURE_2D, gstate->fbo_position);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, screenW, screenH, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glBindTexture(GL_TEXTURE_2D, 0);

    glGenTextures(1, &gstate->rbo_depth);
    glBindTexture(GL_TEXTURE_2D, gstate->rbo_depth);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, screenW, screenH, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, NULL);
    glBindTexture(GL_TEXTURE_2D, 0);

    glGenFramebuffers(1, &gstate->fbo);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, gstate->fbo);
    glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gstate->fbo_texture, 0);
    glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gstate->fbo_normal, 0);
    glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gstate->fbo_position, 0);
    glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, gstate->rbo_depth, 0);

    GLenum DrawBuffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2};
    glDrawBuffers(ARRAY_COUNT(DrawBuffers), DrawBuffers);

    GLenum status;
    if ((status = glCheckFramebufferStatus(GL_FRAMEBUFFER)) != GL_FRAMEBUFFER_COMPLETE) {
      fprintf(stderr, "glCheckFramebufferStatus: error %d", status);
      return;
    }
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
}

void initLights(Permanent_Storage *state)
{
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, state->fplus.lightBuffer);
    PointLight *pointLights = (PointLight*)glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_READ_WRITE);

    for (int i = 0; i < NUM_LIGHTS; i++) {
        PointLight *light = &pointLights[i];
        light->position = vec4((random()%10000)/100.f,0.5f,(random()%10000)/100.f, 1.0f);
        light->color = vec4(1.0f -(random()%1000)/1000.f, 1.0f - (random()%1000)/1000.f, 1.0f - (random()%1000)/1000.f, 1.0f);
        light->paddingAndRadius = vec4FromVec3AndW(vec3(0.0f,0.0f,0.0f), 5.f); // TODO: add light radius
    }
    glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void createParticleSystem(ParticleSystem *particles)
{
    int vertices = particles->maxParticles;

    u32 bufferSize = vertices*(sizeof(Vec3)+sizeof(r32));
    //void* data = malloc(bufferSize);
    glGenBuffers(1, &particles->GLBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, particles->GLBuffer);
    glBufferData(GL_ARRAY_BUFFER, bufferSize, particles->particleData, GL_DYNAMIC_DRAW);
    glGenVertexArrays(1, &particles->VAO);
    glBindVertexArray(particles->VAO);
    //glBindBuffer(GL_ARRAY_BUFFER, 0);
    /*glGenBuffers(1, &mesh->ElementBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->ElementBuffer);
    int byteOffset = vertices*sizeof(Vec4)+vertices*sizeof(Vec3)+vertices*sizeof(Vec2);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh->faces*3*2, ((char*)data)+byteOffset, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);*/
    /*glGenVertexArrays(1, &mesh->VAO);
    glBindVertexArray(mesh->VAO);
    glBindBuffer(GL_ARRAY_BUFFER, mesh->AttribBuffer);*/
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    //glEnableVertexAttribArray(2);
    //glEnableVertexAttribArray(3);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vec3)+sizeof(r32), 0);
    glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, sizeof(Vec3)+sizeof(r32), (GLvoid*)(0+sizeof(Vec3)));
    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->ElementBuffer);
    //glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void renderAndUpdateParticleSystem(ParticleSystem *particles, r32 dt)
{
    float particleSpawnRate = 0.5f;
    particles->timeSinceLastParticle +=dt;
    if(particles->timeSinceLastParticle >= particleSpawnRate)
    {
        particles->particles[particles->lastDestroyedIndex].localPosition =  vec3(0.f,0.f,0.f);
        particles->particles[particles->lastDestroyedIndex].timer = 0.f;
        particles->particles[particles->lastDestroyedIndex].fade = 1.f;
        particles->lastSpawnedIndex = particles->lastDestroyedIndex;
        particles->lastDestroyedIndex = (particles->lastDestroyedIndex+1)%particles->maxParticles;
        //printf("Particle spawned\n");
    }

    for(int i = 0; i < particles->maxParticles; i++)
    {
        particles->particles[i].timer += dt;
        particles->particles[i].fade = 1.f-particles->particles[i].timer/particles->lifeTime;
        Vec3 scaledVel;
        vec3Scale(&scaledVel, &particles->velocity, dt);
        vec3Add(&particles->particles[i].localPosition, &particles->particles[i].localPosition, &scaledVel);
        //particles->particleData[i].position = particles->position+particles->particles[i].localPosition;
        particles->particleData[i].fade = 1.f;
    }

    //printf("P0a pos %f %f %f \n",particles->particleData[0].position.x,particles->particleData[0].position.y,particles->particleData[0].position.z);

    int vertices = particles->maxParticles;
    u32 bufferSize = vertices*(sizeof(Vec3)+sizeof(r32));

    glBindBuffer(GL_ARRAY_BUFFER, particles->GLBuffer);
    glBindVertexArray(particles->VAO);
    glBufferData(GL_ARRAY_BUFFER, bufferSize, &particles->particleData, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glDrawArrays(GL_POINTS, 0, particles->maxParticles);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

PlatformApi Platform;
void init(EngineMemory *mem, int width, int height)
{
    Platform = mem->platformApi;
    memset(mem->gameState, 0, 100LL*1024LL*1024LL);
    Permanent_Storage *state = (Permanent_Storage*)mem->gameState;
    state->deferred = true;

    cameraInitialize(&(state->main_cam));
    state->main_cam.FOV = 45.f;
    state->main_cam.nearPlane = 0.1f;
    state->main_cam.farPlane = 200.f;
    state->numEntities = 0;
    state->numShaders = 0;

    genFrameBuffer(state, width, height);
    GLfloat fbo_vertices[] = {
        -1, -1,
        1, -1,
        -1,  1,
        1,  1,
    };
    // TODO: cleanup this?
    glGenBuffers(1, &state->vbo_fbo_vertices);
    glBindBuffer(GL_ARRAY_BUFFER, state->vbo_fbo_vertices);
    glBufferData(GL_ARRAY_BUFFER, sizeof(fbo_vertices), fbo_vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    genShadowBuffer(state, SHADOWMAP_RES, SHADOWMAP_RES);

    // forward plus stuff
    genZPassBuffer(state, width, height);
    genForwardPlusBuffers(state, width, height);
    initLights(state);

    stackInit((MemStack*)mem->transientState, ((char*)mem->transientState)+sizeof(MemStack), 30LL*1024LL*1024LL);
    state->texturePool = createTexturePool(1024, (MemStack*)mem->transientState);
    state->deferred = true;

    setupDebug();

    InitText();

    domeMesh = loadMesh("sphere.tt");
    mesh = loadMesh("barra/barra.tt");
    terrainMesh = generateTerrainMesh();

    LoadedTexture *groundTexture = loadTextureToMemory(state->texturePool, "grass.png");
    openGL_LoadTexture(groundTexture);
    LoadedTexture *barrelTexture = loadTextureToMemory(state->texturePool, "barra/barra.jpg");
    openGL_LoadTexture(barrelTexture);
    LoadedTexture *stoneTexture = loadTextureToMemory(state->texturePool, "textures/stone.jpg");
    openGL_LoadTexture(stoneTexture);
    LoadedTexture *brickDiffuseTexture = loadTextureToMemory(state->texturePool, "textures/brick_diff.jpg");
    openGL_LoadTexture(brickDiffuseTexture);
    LoadedTexture *brickNormalTexture = loadTextureToMemory(state->texturePool, "textures/brick_norm.jpg");
    openGL_LoadTexture(brickNormalTexture);

    Mesh *planeMesh = generatePlane();

    initializeProgram(&state->textShader,"shaders/textvert.glsl", "shaders/textfrag.glsl", ST_Surface);
    initializeProgram(&state->postProcShader,"shaders/postproc_vert.glsl", "shaders/postproc_frag.glsl", ST_PostProc);
    initializeProgram(&state->postProcPointShader,"shaders/postproc_vert.glsl", "shaders/postproc_point_frag.glsl", ST_PostProc);
    initializeProgram(&state->shadowPassShader,"shaders/vert_shadow_pass.glsl", "shaders/frag_shadow_pass.glsl", ST_Surface);
    initializeComputeProgram(&state->lightCullShader, "shaders/forwardp_lightcull.glsl", ST_LightCull);

    initializeProgram(&state->game.notexShader,"shaders/vert.glsl", "shaders/notex_frag.glsl", ST_Surface);
    initializeProgram(&state->game.texShader,"shaders/vert.glsl", "shaders/frag.glsl", ST_Surface);
    initializeProgram(&state->game.skyShader,"shaders/skydomevert.glsl", "shaders/skydomefrag.glsl", ST_Skydome);
    initializeProgram(&state->game.normalShader,"shaders/vert_normal.glsl", "shaders/frag_normal.glsl", ST_Surface);
    initializeProgram(&state->game.lineShader,"shaders/linevert.glsl", "shaders/linefrag.glsl", ST_Surface);
    initializeProgram(&state->game.particleShader,"shaders/particle_vert.glsl", "shaders/particle_frag.glsl", ST_Particle);

    initializeProgram(&state->game.texShaderForw,"shaders/vert.glsl", "shaders/frag_forward.glsl", ST_Surface);


    addSurfaceShader(state, &state->game.notexShader);
    addSurfaceShader(state, &state->game.texShader);
    addSurfaceShader(state, &state->game.normalShader);
    addSurfaceShader(state, &state->game.texShaderForw);

    meshRecalculateNormals(mesh);
    meshRecalculateNormals(terrainMesh);
    meshRecalculateNormals(planeMesh);
    meshRecalculateNormals(domeMesh);
    openGL_loadMesh(mesh);
    openGL_loadMesh(terrainMesh);
    openGL_loadMesh(planeMesh);
    openGL_loadMesh(domeMesh);

    int k = 0;
    for(int i= 0; i < 60; i++)
    {
        for(int j = 0; j < 60; j++)
        {
            state->game.barrel[k].mesh = *mesh;
            transformInit(&state->game.barrel[k].transform);
            state->game.barrel[k].transform.position =  vec3((float)i+10.0f,0.f,(float)j+10.0f);
            state->game.barrel[k].material.numTextures = 1;
            state->game.barrel[k].material.shader = &state->game.texShader;
            state->game.barrel[k].material.texture_handle[0] = barrelTexture->gl_handle;
            addEntity(state, &state->game.barrel[k]);
            k++;
        }
    }

    state->game.testBarrel.mesh = *mesh/*planeMesh*/;
    transformInit(&state->game.testBarrel.transform);
    state->game.testBarrel.material.numTextures = 1;
    state->game.testBarrel.material.shader = &state->game.texShader;
    state->game.testBarrel.material.texture_handle[0] = barrelTexture->gl_handle;
    addEntity(state, &state->game.testBarrel);

    state->game.terrain.mesh = *terrainMesh;
    transformInit(&state->game.terrain.transform);
    printf("scalex %f",state->game.terrain.transform.scale.x);
    state->game.terrain.transform.position =  vec3(0.f,0.f,0.f);
    state->game.terrain.material.numTextures = 1;
    state->game.terrain.material.shader = &state->game.texShader;
    state->game.terrain.material.texture_handle[0] = groundTexture->gl_handle;
    addEntity(state, &state->game.terrain);

    state->game.wall.mesh = *planeMesh;
    transformInit(&state->game.wall.transform);
    state->game.wall.transform.position =  vec3(10.0f,5.0f,0.0f);
    state->game.wall.material.numTextures = 2;
    state->game.wall.material.shader = &state->game.normalShader;
    state->game.wall.material.texture_handle[0] = brickDiffuseTexture->gl_handle;
    state->game.wall.material.texture_handle[1] = brickNormalTexture->gl_handle;
    addEntity(state, &state->game.wall);

    state->game.wall2.mesh = *planeMesh;
    transformInit(&state->game.wall2.transform);
    state->game.wall2.transform.position =  vec3(9.0f,5.0f,0.0f);
    state->game.wall2.material.numTextures = 2;
    state->game.wall2.material.shader = &state->game.texShader;
    state->game.wall2.material.texture_handle[0] = brickDiffuseTexture->gl_handle;
    addEntity(state, &state->game.wall2);

    state->game.dome.mesh = *domeMesh;
    transformInit(&state->game.dome.transform);
    state->game.dome.transform.position =  vec3(0.0f,0.0f,0.0f);;
    state->game.dome.material.numTextures = 0;
    state->game.dome.material.shader = &state->game.skyShader;
    //dome.transform.scale = Vec3(3500.0f,3500.0f,3500.0f);
    state->game.dome.transform.scale =  vec3(200.0f,200.0f,200.0f);
    //addEntity(&dome);

    state->game.earth.mesh = *domeMesh;
    transformInit(&state->game.earth.transform);
    state->game.earth.transform.position =  vec3(0.0f,0.0f,0.0f);;
    state->game.earth.material.numTextures = 0;
    state->game.earth.material.shader = &state->game.texShader;
    state->game.earth.material.numTextures = 1;
    state->game.earth.material.texture_handle[0] = stoneTexture->gl_handle;
    //earth.transform.scale =  vec3(3000.0f,3000.0f,3000.0f);
    float planetSize = 15000.f;
    state->game.earth.transform.scale =  vec3(planetSize,planetSize,planetSize);
    //addEntity(&earth);

    state->game.particles.lifeTime = 10.f;
    state->game.particles.maxParticles = 1000;
    state->game.particles.lastSpawnedIndex = 0;
    state->game.particles.position =  vec3(10.f,0.f,10.f);
    state->game.particles.texture = *barrelTexture;
    state->game.particles.velocity =  vec3(0.2f,1.0f,0.0f);
    state->game.particles.lastDestroyedIndex = 0;
    state->game.particles.lastSpawnedIndex = 0;
    state->game.particles.timeSinceLastParticle = 0.0f;
    createParticleSystem(&state->game.particles);



    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);

    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);
    glDepthFunc(GL_LEQUAL);
    glDepthRange(0.0f, 1.0f);

    if(!glCreateShader(GL_GEOMETRY_SHADER)){
        printf("Creating geos failed\n");
    }

    //glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

int frames = 0;

void visibilityCheck(Permanent_Storage *state)
{
    Plane frustumPlanes[6];
    getFrustumPlanes(&state->main_cam.perspectiveMatrix,frustumPlanes);
    for(int i = 0; i < 6; i++)
    {
        Vec4 thing = vec4(frustumPlanes[i].a,frustumPlanes[i].b,frustumPlanes[i].c,0.0f);
        Mat4 mat = cameraCalculateInverseViewMatrix(&state->main_cam);
        Vec4 res;
        mat4Vec4Mul(&res, &mat, &thing);
        frustumPlanes[i] = planeFromVec4(&res);
    }

    Vec3 n[6];
    r32 d[6];

    for(int planeId = 0; planeId < 6; planeId++)
    {
        Vec3 idk = vec3(frustumPlanes[planeId].a,frustumPlanes[planeId].b,frustumPlanes[planeId].c);
        n[planeId] =  vec3Normalized(&idk);
        d[planeId] = frustumPlanes[planeId].d;
    }

    for(int i = 0; i < state->numEntities; i++)
    {
        // TODO: check for all 6 planes?
        Vec3 pos;
        vec3Sub(&pos, &state->entities[i]->transform.position, &state->main_cam.position);
        r32 res = vec3Dot(&pos, &n[0]) + d[0]+state->entities[i]->mesh.boundingRadius;
        r32 res2 = vec3Dot(&pos, &n[1]) + d[1]+state->entities[i]->mesh.boundingRadius;
        if(res <= 0.f || res2 <= 0.f)
            state->entities[i]->visible = false;
        else
            state->entities[i]->visible = true;
    }
}

void deferredRender(Permanent_Storage *state, Input *input, float dt)
{
    Mat4 modelMatrix;
    Mat4 posToShadowSpace;


    // ------------------------- SHADOW PASS ------------------------------------------

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, state->shadow_fbo);
    glViewport(0, 0, (GLsizei) SHADOWMAP_RES, (GLsizei) SHADOWMAP_RES);

    //glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClearDepth(1.0f);
    glClear(GL_DEPTH_BUFFER_BIT);

    Quaternion q = quaternion(-state->game.sunRot2Quat.w,state->game.sunRot2Quat.x,state->game.sunRot2Quat.y,state->game.sunRot2Quat.z);
    Mat4 lightTransformMatrix;
    mat4FromQuaternion(&lightTransformMatrix, &q);

    Mat4 specialPerspective = projMatrix(state->main_cam.FOV, state->main_cam.aspectRatio, state->main_cam.nearPlane, 20.f);
    Mat4 mat = cameraCalculateInverseViewMatrix(&state->main_cam);
    Mat4 mat2 = invPerspective(&specialPerspective);
    Mat4 camInvPerspective;
    mat4Mul(&camInvPerspective, &mat, &mat2);

    Vec3 frust[8];
    //left
        // down
    frust[0]=  vec3(-1.f,-1.f,-1.f);
    frust[1]=  vec3(-1.f,-1.f,1.f);
        // up
    frust[2]= vec3(-1.f,1.f,-1.f);
    frust[3]= vec3(-1.f,1.f,1.f);
    //right
    frust[4]= vec3(1.f,-1.f,-1.f);
    frust[5]= vec3(1.f,-1.f,1.f);
    frust[6]= vec3(1.f,1.f,-1.f);
    frust[7]= vec3(1.f,1.f,1.f);

    r32 minX = 99999999.f;
    r32 minY = 99999999.f;
    r32 minZ = 99999999.f;
    r32 maxX = -99999999.f;
    r32 maxY = -99999999.f;
    r32 maxZ = -99999999.f;

    for(int i = 0; i < 8; i++)
    {
        Vec4 row = vec4FromVec3AndW(frust[i],1.0f);
        Vec4 sdf;
        mat4Vec4Mul(&sdf, &camInvPerspective, &row);
        vec4Scale(&sdf, &sdf, sdf.w);
        frust[i] = vec3FromVec4(sdf);
        minX = min(minX, frust[i].x);
        minY = min(minY, frust[i].y);
        minZ = min(minZ, frust[i].z);
        maxX = max(maxX, frust[i].x);
        maxY = max(maxY, frust[i].y);
        maxZ = max(maxZ, frust[i].z);
    }

    //drawLine(frust[1]*0.99f,frust[2]*0.99f);

    /*static Vec3 thing[8];

    for(int i = 0; i < 8;i++)
    {
        for(int j = 0; j < 8; j++)
        {
            drawLine(thing[i],thing[j]);
        }
    }*/

    /*if(getKeyDown(input, KEYCODE_F))
    {
        thing[0] = Vec3(minX,minY,minZ);
        thing[1] = Vec3(minX,minY,maxZ);
        thing[2] = Vec3(minX,maxY,minZ);
        thing[3] = Vec3(minX,maxY,maxZ);
        thing[4] = Vec3(maxX,minY,minZ);
        thing[5] = Vec3(maxX,minY,maxZ);
        thing[6] = Vec3(maxX,maxY,minZ);
        thing[7] = Vec3(maxX,maxY,maxZ);

        setPosition(&state->game.barrel[0].transform,Vec3(minX,minY,minZ));
        setPosition(&state->game.barrel[1].transform,Vec3(minX,minY,maxZ));
        setPosition(&state->game.barrel[2].transform,Vec3(minX,maxY,minZ));
        setPosition(&state->game.barrel[3].transform,Vec3(minX,maxY,maxZ));
        setPosition(&state->game.barrel[4].transform,Vec3(maxX,minY,minZ));
        setPosition(&state->game.barrel[5].transform,Vec3(maxX,minY,maxZ));
        setPosition(&state->game.barrel[6].transform,Vec3(maxX,maxY,minZ));
        setPosition(&state->game.barrel[7].transform,Vec3(maxX,maxY,maxZ));
    }*/

    Mat4 orthoMat = ortho(minX, maxX, minY, maxY, minZ, maxZ);

    Mat4 civm = cameraCalculateInverseViewMatrix(&state->main_cam);
    mat4Mul(&posToShadowSpace, &orthoMat, &lightTransformMatrix);
    mat4Mul(&posToShadowSpace, &posToShadowSpace, &civm);

    glUseProgram(state->shadowPassShader.program);
    for(int i = 0; i < state->numEntities; i++)
    {
        if(state->entities[i]->visible)
        {
            modelMatrix = *calculateModelMatrix(&state->entities[i]->transform);
            mat4Mul(&modelMatrix, &lightTransformMatrix, &modelMatrix);
            glUniformMatrix4fv(state->shadowPassShader.surface.perspectiveMatrixUnif, 1, GL_FALSE, (const GLfloat*)&orthoMat);
            glUniformMatrix4fv(state->shadowPassShader.surface.transformMatrixUnif, 1, GL_FALSE, (const GLfloat*)&modelMatrix);
            renderMesh(&state->entities[i]->mesh);
        }
    }
    glUseProgram(0);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

    // SHADOW PASS END

    //glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, state->fbo);
    glViewport(0, 0, (GLsizei) state->windowWidth, (GLsizei)state->windowHeight);

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClearDepth(1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    GLuint prevProgram = 0xFFFFFFFF;
    GLuint prog;
    for(int i = 0; i < state->numEntities; i++)
    {
        if(state->entities[i]->visible)
        {
            prog = state->entities[i]->material.shader->program;
            if(prog != prevProgram)
            {
                glUseProgram(state->entities[i]->material.shader->program);
                if(state->entities[i]->material.shader->type == ST_Surface)
                {
                    Vec4 ldir = vec4(0.866f, 0.5f,0.0f,0.0f);
                    Vec4 tempLightDir;
                    mat4Vec4Mul(&tempLightDir, &state->main_cam.transformMatrix, &ldir);
                    glUniform4fv(state->game.texShader.surface.lightDirUnif, 1, (const GLfloat*)&tempLightDir);
                }
                else if(state->entities[i]->material.shader->type == ST_Skydome)
                {
                    modelMatrix = *calculateModelMatrix(&state->entities[i]->transform);
                    mat4Mul(&modelMatrix, &state->main_cam.transformMatrix, &modelMatrix);
                }
            }
            modelMatrix = *calculateModelMatrix(&state->entities[i]->transform);
            mat4Mul(&modelMatrix, &state->main_cam.transformMatrix, &modelMatrix);

            glUniformMatrix4fv(state->game.texShader.surface.transformMatrixUnif, 1, GL_FALSE, (const GLfloat*) &modelMatrix);

            glUniform1i(state->entities[i]->material.shader->surface.diffuseTexture, 0);
            glUniform1i(state->entities[i]->material.shader->surface.normalTexture, 1);

            for(int textureId = 0; textureId < state->entities[i]->material.numTextures; textureId++)
            {
                glActiveTexture(GL_TEXTURE0+textureId);
                glBindTexture(GL_TEXTURE_2D, state->entities[i]->material.texture_handle[textureId]);
            }
            renderMesh(&state->entities[i]->mesh);
            prevProgram = prog;
        }
    }

    drawDebugLines(state->game.lineShader, &state->main_cam);

    glBindVertexArray(0);

    glBindFramebuffer(GL_READ_FRAMEBUFFER, state->fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glClearColor(0.0, 0.0, 0.0, 0.0);
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE);

    // postprocessing quad
    glBindBuffer(GL_ARRAY_BUFFER, state->vbo_fbo_vertices);
    glVertexAttribPointer(
                0,                  // attribute
                2,                  // number of elements per vertex, here (x,y)
                GL_FLOAT,           // the type of each element
                GL_FALSE,           // take our values as-is
                0,                  // no extra data between each position
                0                   // offset of first element
                );

    // light passes

    // directional light
    glUseProgram(state->postProcShader.program);
    glUniform1i(state->postProcShader.postproc.albedo_unif, 0);
    glUniform1i(state->postProcShader.postproc.depth_unif,  1);
    glUniform1i(state->postProcShader.postproc.normal_unif, 2);
    glUniform1i(state->postProcShader.postproc.position_unif, 3);
    glUniform1i(state->postProcShader.postproc.shadow_unif, 4);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, state->fbo_texture);
    glActiveTexture(GL_TEXTURE0+1);
    glBindTexture(GL_TEXTURE_2D, state->rbo_depth);
    glActiveTexture(GL_TEXTURE0+2);
    glBindTexture(GL_TEXTURE_2D, state->fbo_normal);
    glActiveTexture(GL_TEXTURE0+3);
    glBindTexture(GL_TEXTURE_2D, state->fbo_position);
    glActiveTexture(GL_TEXTURE0+4);
    glBindTexture(GL_TEXTURE_2D, state->shadow_fbo_depth);
    glEnableVertexAttribArray(0); // TODO

    // TODO
    Vec4 transformedSunDir;
    mat4Vec4Mul(&transformedSunDir, &state->main_cam.transformMatrix, &state->game.sunDir);
    glUniform4fv(state->postProcShader.postproc.light_unif, 1, (const GLfloat*)&transformedSunDir);
    glUniformMatrix4fv(state->postProcShader.postproc.lightProjM_unif, 1, GL_FALSE, (const GLfloat*)&posToShadowSpace);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    // point light
    glUseProgram(state->postProcPointShader.program);
    glUniform1i(state->postProcPointShader.postproc.albedo_unif, 0);
    glUniform1i(state->postProcPointShader.postproc.depth_unif,  1);
    glUniform1i(state->postProcPointShader.postproc.normal_unif, 2);
    glUniform1i(state->postProcPointShader.postproc.position_unif, 3);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, state->fbo_texture);
    glActiveTexture(GL_TEXTURE0+1);
    glBindTexture(GL_TEXTURE_2D, state->rbo_depth);
    glActiveTexture(GL_TEXTURE0+2);
    glBindTexture(GL_TEXTURE_2D, state->fbo_normal);
    glActiveTexture(GL_TEXTURE0+3);
    glBindTexture(GL_TEXTURE_2D, state->fbo_position);
    glEnableVertexAttribArray(0);
    //Vec4 pointLight(state->main_cam.transformMatrix*Vec4(frames*0.01f,0.7f,frames*0.01f,1.0f));
    //glUniform4fv(postProcPointShader.postproc.light_unif, 1, (const GLfloat*)&pointLight);
    //glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);0

    glDisable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER,0);
    glUseProgram(0);

    //SKY
    float outerR = 20000.0f;
    float innerR = 18000.0f;
    float outerR2 = outerR*outerR;
    float innerR2 = innerR*innerR;
    float mieMult = 0.005f;
    float rayMult = 0.003f;
    setPosition(&state->game.dome.transform, vec3(state->main_cam.position.x,-innerR/100.f+state->main_cam.position.y,state->main_cam.position.z));
    float cheight = innerR+100.0f;
    updateScaleMatrix(&state->game.dome.transform);
    glUseProgram(state->game.skyShader.program);

    modelMatrix = *calculateModelMatrix(&state->game.dome.transform);
    mat4Mul(&modelMatrix, &state->main_cam.transformMatrix, &modelMatrix);
    Mat4 domeScale = scale( vec3(20000.f,20000.f,20000.f));
    glUniformMatrix4fv( state->game.skyShader.skydome.scaleMatrix, 1, GL_FALSE, (const GLfloat*)&domeScale);
    glUniformMatrix4fv( state->game.skyShader.skydome.perspectiveMatrix, 1, GL_FALSE, (const GLfloat*) &state->main_cam.perspectiveMatrix);
    glUniformMatrix4fv( state->game.skyShader.skydome.transformMatrix,1,GL_FALSE, (const GLfloat*) &modelMatrix);
    Vec3 camPos = vec3(0.0f,innerR,0.0f);
    glUniform3fv(       state->game.skyShader.skydome.v3CameraPos          ,1,(const GLfloat*)&camPos);
    glUniform3fv(       state->game.skyShader.skydome.v3LightDir     		,1,(const GLfloat*)&state->game.sunDir);
    glUniform3f(        state->game.skyShader.skydome.v3InvWavelength, 1.0 / pow(0.650, 4.0), 1.0 / pow(0.570, 4.0), 1.0 / pow(0.475, 4.0));
    glUniform1fARB(        state->game.skyShader.skydome.fCameraHeight  	    , cheight);
    glUniform1fARB(        state->game.skyShader.skydome.fCameraHeight2        , (float)pow(cheight,2.0));
    glUniform1fARB(        state->game.skyShader.skydome.fOuterRadius    	    , outerR);
    glUniform1fARB(        state->game.skyShader.skydome.fOuterRadius2         , outerR2);
    glUniform1fARB(        state->game.skyShader.skydome.fInnerRadius          , innerR);
    glUniform1fARB(        state->game.skyShader.skydome.fInnerRadius2         , innerR2);
    glUniform1fARB(        state->game.skyShader.skydome.fKrESun     	        , rayMult*15.0f);//
    glUniform1fARB(        state->game.skyShader.skydome.fKmESun               , mieMult*15.0f);//
    glUniform1fARB(        state->game.skyShader.skydome.fKr4PI                , rayMult*4.0f*3.142f);
    glUniform1fARB(        state->game.skyShader.skydome.fKm4PI                , mieMult*4.0f*3.142f);
    glUniform1fARB(        state->game.skyShader.skydome.fScale                , 1.f / (outerR - innerR));//
    glUniform1fARB(        state->game.skyShader.skydome.fScaleOverScaleDepth  , (1.0f / (outerR - innerR)) / 0.25f);//
    glDisable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    renderMesh(&state->game.dome.mesh);
    glEnable(GL_CULL_FACE);
    glDisable(GL_BLEND);
    glUseProgram(0);
}

void forwardRender(Permanent_Storage *state, Input *input, float dt)
{
    //setPosition(&state->game.testBarrel.transform,state->game.testBarrel.transform.position+ vec3(0.01f,0.0f,0.01f));

    glDisable(GL_BLEND);
    //glBlendFunc(GL_ONE, GL_ONE);

    Mat4 modelMatrix;

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, state->zprepass_fbo);
    //glViewport(0, 0, (GLsizei) SHADOWMAP_RES, (GLsizei) SHADOWMAP_RES);
    glClearDepth(1.0f);
    glClear(GL_DEPTH_BUFFER_BIT);

    // Z PREPASS
    glUseProgram(state->shadowPassShader.program);
    glUniformMatrix4fv(state->shadowPassShader.surface.perspectiveMatrixUnif, 1, GL_FALSE, (const GLfloat*) &state->main_cam.perspectiveMatrix);
    for(int i = 0; i < state->numEntities; i++)
    {
        if(state->entities[i]->visible)
        {
            modelMatrix = *calculateModelMatrix(&state->entities[i]->transform);
            mat4Mul(&modelMatrix, &state->main_cam.transformMatrix, &modelMatrix);
            glUniformMatrix4fv(state->shadowPassShader.surface.transformMatrixUnif, 1, GL_FALSE, (const GLfloat*) &modelMatrix);
            renderMesh(&state->entities[i]->mesh);
        }
    }
    glUseProgram(0);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // LIGHT CULLING

    glUseProgram(state->lightCullShader.program);
    glUniformMatrix4fv(state->lightCullShader.lightc.projection, 1, GL_FALSE, (const GLfloat*) &state->main_cam.perspectiveMatrix);
    glUniformMatrix4fv(state->lightCullShader.lightc.view, 1, GL_FALSE, (const GLfloat*) &state->main_cam.transformMatrix);

    glUniform1i(state->lightCullShader.lightc.lightCount, NUM_LIGHTS);
    glUniform2i(state->lightCullShader.lightc.screenSize, state->windowWidth, state->windowHeight);

    glActiveTexture(GL_TEXTURE4);
    glUniform1i(state->lightCullShader.lightc.depthMap, 4);
    glBindTexture(GL_TEXTURE_2D, state->zprepass_fbo_depth);

    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, state->fplus.lightBuffer);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, state->fplus.visibleLightIndicesBuffer);
    glDispatchCompute(state->fplus.workGroupsX, state->fplus.workGroupsY, 1);

    glBindTexture(GL_TEXTURE_2D, 0);
    glUseProgram(0);

    // RENDER

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClearColor(1.0, 0.0, 0.0, 0.0);
    glClearDepth(1.0f);
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, state->fplus.lightBuffer);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, state->fplus.visibleLightIndicesBuffer);

    GLuint prevProgram = 0xFFFFFFFF;
    GLuint prog;
    for(int i = 0; i < state->numEntities; i++)
    {
        if(state->entities[i]->visible)
        {
            prog = state->entities[i]->material.shader->program;
            if(prog != prevProgram)
            {
                glUseProgram(state->entities[i]->material.shader->program);

                if(state->entities[i]->material.shader->type == ST_Surface)
                {
                    glUniformMatrix4fv(state->entities[i]->material.shader->surface.viewMatixUnif, 1, GL_FALSE, (const GLfloat*) &state->main_cam.transformMatrix);
                }
            }
            modelMatrix = *calculateModelMatrix(&state->entities[i]->transform);
            mat4Mul(&modelMatrix, &state->main_cam.transformMatrix, &modelMatrix);
            glUniformMatrix4fv(state->entities[i]->material.shader->surface.transformMatrixUnif, 1, GL_FALSE, (const GLfloat*) &modelMatrix);

            glUniform1i(state->entities[i]->material.shader->surface.diffuseTexture, 0);
            glUniform1i(state->entities[i]->material.shader->surface.normalTexture, 1);

            for(int textureId = 0; textureId < state->entities[i]->material.numTextures; textureId++)
            {
                glActiveTexture(GL_TEXTURE0+textureId);
                glBindTexture(GL_TEXTURE_2D, state->entities[i]->material.texture_handle[textureId]);
            }
            renderMesh(&state->entities[i]->mesh);
            prevProgram = prog;
        }
    }
    //glBindBuffe
}

void display(EngineMemory *mem, Input *input, float dt)
{
    Permanent_Storage *state = (Permanent_Storage*)mem->gameState;

    float camSpeed = 10.f;
    Vec3 forward = cameraCalculateForwardDirection(&state->main_cam);
    Vec3 right = cameraCalculateRightDirection(&state->main_cam);

    r32 rspeed = dt*camSpeed;

    if(getKey(input, KEYCODE_W))
    {
        Vec3 scaledForw;
        vec3Scale(&scaledForw, &forward, rspeed);
        vec3Add(&state->main_cam.position,&state->main_cam.position,&scaledForw);
        //state->main_cam.position += forward*dt*camSpeed;
    }
    if(getKey(input, KEYCODE_S))
    {
        Vec3 scaledForw;
        vec3Scale(&scaledForw, &forward, rspeed);
        vec3Sub(&state->main_cam.position,&state->main_cam.position,&scaledForw);
    }
    if(getKey(input, KEYCODE_A))
    {
        Vec3 scaledRight;
        vec3Scale(&scaledRight, &right, rspeed);
        vec3Sub(&state->main_cam.position,&state->main_cam.position,&scaledRight);
    }
    if(getKey(input, KEYCODE_D))
    {
        Vec3 scaledRight;
        vec3Scale(&scaledRight, &right, rspeed);
        vec3Add(&state->main_cam.position,&state->main_cam.position,&scaledRight);
    }

    Quaternion movX = quaternionFromAxisAngle(vec3(0.f,1.f,0.f),((float)-input->mousePosition.x)*PI/180.f);
    Quaternion movY = quaternionFromAxisAngle( vec3(1.f,0.f,0.f),((float)-input->mousePosition.y)*PI/180.f);
    quaternionMul(&state->main_cam.rotation, &movX, &movY);

    cameraRecalculateMatrices(&state->main_cam);

    Quaternion sr = quaternionFromAxisAngle( vec3(1.0f,0.0f,0.0f),0.785f);
    Quaternion sr2 = quaternionFromAxisAngle( vec3(1.0f,0.0f,0.0f),3.14+0.785f);

    Vec4 sunDir= vec4(0.0,0.0,-1.0,0.0);
    Mat4 sunRot;
    mat4FromQuaternion(&sunRot, &sr);
    mat4FromQuaternion(&state->game.sunRot2, &sr2);
    state->game.sunRot2Quat = sr2;
    mat4Vec4Mul(&sunDir, &sunRot, &sunDir);

    state->game.wall.transform.rotation = quaternionFromAxisAngle( vec3(0.f,1.f,0.f),((float)frames/100.0f));
    state->game.wall2.transform.rotation = quaternionFromAxisAngle( vec3(0.f,1.f,0.f),((float)frames/100.0f));
    state->game.wall.transform.dirty = true;
    state->game.wall2.transform.dirty = true;
    setListenerTransform(state->main_cam.position, cameraCalculateForwardDirection(&state->main_cam), cameraCalculateUpDirection(&state->main_cam));

    state->game.sunDir = sunDir;

    if(getKeyDown(input, KEYCODE_P))
    {
        //printf("texShader: %d\n",state->game.texShader.program);
        //printf("texShaderForw: %d\n",state->game.texShaderForw.program);
        state->deferred = !state->deferred;
        if(state->deferred)
        {
            for(int i = 0; i < state->numEntities; i++)
            {
                state->entities[i]->material.shader = &state->game.texShader;
            }
        }
        else
        {
            for(int i = 0; i < state->numEntities; i++)
            {
                state->entities[i]->material.shader = &state->game.texShaderForw;
            }
        }
    }

    //glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    //glClearDepth(1.0f);
    //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    visibilityCheck(state); // frustum culling
    if(state->deferred)
    {
        deferredRender(state, input, dt);
    }
    else
    {
        forwardRender(state, input, dt);
        glUseProgram(state->game.particleShader.program);
        glUniformMatrix4fv(state->game.particleShader.surface.perspectiveMatrixUnif, 1, GL_FALSE, (const GLfloat*)&state->main_cam.perspectiveMatrix);
        glUniformMatrix4fv(state->game.particleShader.surface.transformMatrixUnif, 1, GL_FALSE, (const GLfloat*)&state->main_cam.transformMatrix);
        renderAndUpdateParticleSystem(&state->game.particles,dt);
    }

    /*glEnable(GL_BLEND);
    glActiveTexture(GL_TEXTURE0);
    Mat4 textProjection = ortho(0.0f, (float)state->windowWidth, 0.0f, (float)state->windowHeight,-100.f,100.f);
    glUseProgram(state->textShader.program);
    glUniformMatrix4fv(state->textShader.surface.perspectiveMatrixUnif, 1, GL_FALSE, (const GLfloat*)&textProjection);
    glUniform3f(glGetUniformLocation(state->textShader.program, "textColor"), 1.f, 1.f, 1.f);
    char buf[512];
    sprintf(buf,"frametime2: %fms (%d fps)",dt*1000.f,(int)(1.0f/dt));
    RenderText(buf, 10.f, state->windowHeight-25.0f, 0.5f);
    sprintf(buf,"pos: %f %f %f",state->main_cam.position.x,state->main_cam.position.y,state->main_cam.position.z);
    RenderText(buf, 10.f, state->windowHeight-45.0f, 0.5f);

    glDisable(GL_BLEND);
    glUseProgram(0);

    GLenum err;
    //char errBuf[256];
    while ((err = glGetError()) != GL_NO_ERROR) {
        //gluErrorString(err)
        printf("OpenGL error: %d (0x%08x)\n",err,err);
    }*/


    frames++;
}

void reshape (EngineMemory *mem, int w, int h)
{
    Permanent_Storage *state = (Permanent_Storage*)mem->gameState;

    deleteFrameBuffer(state);
    genFrameBuffer(state, w, h);

    if(state->windowWidth == w && state->windowHeight == h)
        return;
    state->windowWidth = w;
    state->windowHeight = h;

    state->main_cam.aspectRatio = (r32)w/(r32)h;
    cameraRecalculateMatrices(&state->main_cam);

    for(u32 i = 0; i < state->numShaders; i++)
    {
        if(state->shaders[i]->type != ST_Surface)
            continue;
        glUseProgram(state->shaders[i]->program);
        glUniformMatrix4fv(state->shaders[i]->surface.perspectiveMatrixUnif, 1, GL_FALSE, (const GLfloat*)&state->main_cam.perspectiveMatrix);
        glUseProgram(0);
    }

    glViewport(0, 0, (GLsizei) w, (GLsizei) h);
}

void gameExit(EngineMemory *game_memory)
{
    audioExit();
}
