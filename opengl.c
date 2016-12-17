#include "engine.h"
#include "core.h"

#include "math.h"

void openglInitializeTerrainGeneration(TerrainGeneratorState* tgstate, u32 maxGroups, u32 cubesPerSeed, r32 voxelScale)
{
    tgstate->voxelScale = voxelScale;
    u32 seedBufferSize = maxGroups*maxGroups*maxGroups*sizeof(Vec4);

    glGenBuffers(1, &tgstate->vertInbuffer);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, tgstate->vertInbuffer);
    glBufferData(GL_SHADER_STORAGE_BUFFER, seedBufferSize, 0, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

    tgstate->tunnelData.tunnelCount = 0;
    // DOEST WORK
    tgstate->tunnelData.firstOctaveMax = 4.5f;
    tgstate->tunnelData.secondOctaveMax = 20.6f;

    glGenBuffers(1, &tgstate->tunnelBuffer);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, tgstate->tunnelBuffer);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(ChunkGenData), &tgstate->tunnelData, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

    glGenBuffers(1, &tgstate->edgeVertexBuffer);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, tgstate->edgeVertexBuffer);
    u32 workgourpsPerChunk = CHUNK_SIZE/CHUNK_WORKGROUP_SIZE;
    int workGroups = workgourpsPerChunk * workgourpsPerChunk * workgourpsPerChunk ;
    u32 bufferSize = workGroups*(CHUNK_WORKGROUP_SIZE+1)*(CHUNK_WORKGROUP_SIZE+1)*(CHUNK_WORKGROUP_SIZE+1)*sizeof(Vec4)*3;
    tgstate->edgeVertexBufferSize = bufferSize;
    glBufferData(GL_SHADER_STORAGE_BUFFER, bufferSize, 0, GL_STATIC_DRAW);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
    printf("Buffer size %dMB\n",bufferSize/(u32)Megabytes(1));

    glGenBuffers(1, &tgstate->vertAtomicBuffer);
    glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, tgstate->vertAtomicBuffer);
    glBufferData(GL_ATOMIC_COUNTER_BUFFER, sizeof(GLuint)*2, NULL, GL_DYNAMIC_READ);
    glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, 0);

    tgstate->initialized = true;
}

void openglPrepageTerrainGeneration(TerrainGeneratorState* tgstate, GLuint outBuffer, GLuint outElementBuffer, u32 groups, r32 scale)
{
    assert(tgstate->initialized);

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, outBuffer);
    // TODO: is this guaranteed to not reallocate if we always use same size even on buffer previously created?
    glBufferData(GL_SHADER_STORAGE_BUFFER, (CHUNK_VERTEX_BUFFER_SIZE/(CHUNK_SIZE/CHUNK_WORKGROUP_SIZE))*groups, 0, GL_DYNAMIC_COPY);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, outElementBuffer);
    glBufferData(GL_SHADER_STORAGE_BUFFER, (CHUNK_ELEMENT_BUFFER_SIZE/(CHUNK_SIZE/CHUNK_WORKGROUP_SIZE))*groups, 0, GL_DYNAMIC_COPY);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

    u32 seedBufferSize = groups*groups*groups*sizeof(Vec4);
    Vec4* seedVerts = (Vec4*)alloca(seedBufferSize);
    r32 voxelScale = scale;
    int index = 0;

    for(int k = 0; k < groups; k++)
    {
        for(int i = 0; i < groups; i++)
        {
            for(int j = 0; j < groups; j++)
            {

                seedVerts[index] = vec4(i*voxelScale*CHUNK_WORKGROUP_SIZE,
                                            k*voxelScale*CHUNK_WORKGROUP_SIZE, j*voxelScale*CHUNK_WORKGROUP_SIZE, 1.0);
                index++;
            }
        }
    }

    // update seed vertices
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, tgstate->vertInbuffer);
    glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, seedBufferSize, (GLvoid*)seedVerts);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

    // reset atomic counters
    GLuint* counter;
    glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, tgstate->vertAtomicBuffer);
    counter = (GLuint*)glMapBufferRange(GL_ATOMIC_COUNTER_BUFFER, 0, sizeof(GLuint)*2,
                                             GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT | GL_MAP_UNSYNCHRONIZED_BIT
                                             );
    memset(counter, 0, sizeof(GLuint)*2);
    glUnmapBuffer(GL_ATOMIC_COUNTER_BUFFER);
    glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, 0);

    // update tunnel data;
    //printf("first was %f",tgstate->tunnelData.firstOctaveMax);
    glBindBuffer(GL_ARRAY_BUFFER, tgstate->tunnelBuffer);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(ChunkGenData), &tgstate->tunnelData);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    int glerror = glGetError();
    if(glerror != 0)
    {
        printf("glerror: %d\n",glerror);
        __builtin_trap();
    }
}

GLuint opengl_Int16Texture2D(u32 width, u32 height, void* data)
{
    GLuint result;
    glGenTextures(1, &result);
    glBindTexture(GL_TEXTURE_2D, result);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA16I_EXT, 16, 256, 0, GL_ALPHA_INTEGER_EXT, GL_INT, data);
    glBindTexture(GL_TEXTURE_2D, 0);
    return result;
}

GLuint opengl_Int16Texture1D(u32 width, void* data)
{
    GLuint result;
    glGenTextures(1, &result);
    glBindTexture(GL_TEXTURE_1D, result);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexImage1D(GL_TEXTURE_1D, 0, GL_ALPHA16I_EXT, width, 0, GL_ALPHA_INTEGER_EXT, GL_INT, data);
    glBindTexture(GL_TEXTURE_1D, 0);
    return result;
}

void openglCreateDepthFBO(OpenglFrameBuffer *framebuffer, int width, int height, b32 isShadowMap)
{
    framebuffer->textureCount = 1;

    glActiveTexture(GL_TEXTURE0);
    glGenTextures(1, &framebuffer->depthFBO.depthTexture);
    glBindTexture(GL_TEXTURE_2D, framebuffer->depthFBO.depthTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    if(isShadowMap)
    {
        GLfloat cols[4];
        cols[0] = 1.0;
        cols[1] = 1.0;
        cols[2] = 1.0;
        cols[3] = 1.0;
        glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, cols);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    }
    else
    {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    }
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, NULL);
    glBindTexture(GL_TEXTURE_2D, 0);

    glGenFramebuffers(1, &framebuffer->fboHandle);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, framebuffer->fboHandle);
    glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, framebuffer->depthFBO.depthTexture, 0);

    GLenum status;
    if ((status = glCheckFramebufferStatus(GL_FRAMEBUFFER)) != GL_FRAMEBUFFER_COMPLETE) {
      printf("glCheckFramebufferStatus: error %d", status);
      return;
    }
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
}

void openglCreateDeferredFBO(OpenglFrameBuffer *framebuffer, u32 screenW, u32 screenH)
{
    framebuffer->textureCount = 4;

    glActiveTexture(GL_TEXTURE0);
    glGenTextures(1, &framebuffer->deferredFBO.diffuseTexture);
    glBindTexture(GL_TEXTURE_2D, framebuffer->deferredFBO.diffuseTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, screenW, screenH, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glBindTexture(GL_TEXTURE_2D, 0);

    glGenTextures(1, &framebuffer->deferredFBO.normalTexture);
    glBindTexture(GL_TEXTURE_2D, framebuffer->deferredFBO.normalTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, screenW, screenH, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glBindTexture(GL_TEXTURE_2D, 0);

    glGenTextures(1, &framebuffer->deferredFBO.positionTexture);
    glBindTexture(GL_TEXTURE_2D, framebuffer->deferredFBO.positionTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, screenW, screenH, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glBindTexture(GL_TEXTURE_2D, 0);

    glGenTextures(1, &framebuffer->deferredFBO.depthTexture);
    glBindTexture(GL_TEXTURE_2D, framebuffer->deferredFBO.depthTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, screenW, screenH, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, NULL);
    glBindTexture(GL_TEXTURE_2D, 0);

    glGenFramebuffers(1, &framebuffer->fboHandle);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, framebuffer->fboHandle);
    glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, framebuffer->deferredFBO.diffuseTexture, 0);
    glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, framebuffer->deferredFBO.normalTexture, 0);
    glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, framebuffer->deferredFBO.positionTexture, 0);
    glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, framebuffer->deferredFBO.depthTexture, 0);

    GLenum DrawBuffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2};
    glDrawBuffers(ARRAY_COUNT(DrawBuffers), DrawBuffers);

    GLenum status;
    if ((status = glCheckFramebufferStatus(GL_FRAMEBUFFER)) != GL_FRAMEBUFFER_COMPLETE) {
      printf("glCheckFramebufferStatus: error %d", status);
      return;
    }
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
}

void openglCreateForwardFBO(OpenglFrameBuffer *framebuffer, u32 screenW, u32 screenH)
{
    framebuffer->textureCount = 2;

    glActiveTexture(GL_TEXTURE0);
    glGenTextures(1, &framebuffer->forwardFBO.diffuseTexture);
    glBindTexture(GL_TEXTURE_2D, framebuffer->forwardFBO.diffuseTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, screenW, screenH, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glBindTexture(GL_TEXTURE_2D, 0);

    glGenTextures(1, &framebuffer->forwardFBO.depthTexture);
    glBindTexture(GL_TEXTURE_2D, framebuffer->forwardFBO.depthTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32_ARB, screenW, screenH, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, NULL);
    glBindTexture(GL_TEXTURE_2D, 0);

    glGenFramebuffers(1, &framebuffer->fboHandle);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, framebuffer->fboHandle);
    glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, framebuffer->forwardFBO.diffuseTexture, 0);
    glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, framebuffer->forwardFBO.depthTexture, 0);

    GLenum DrawBuffers[] = { GL_COLOR_ATTACHMENT0 };
    glDrawBuffers(ARRAY_COUNT(DrawBuffers), DrawBuffers);

    GLenum status;
    if ((status = glCheckFramebufferStatus(GL_FRAMEBUFFER)) != GL_FRAMEBUFFER_COMPLETE) {
      printf("glCheckFramebufferStatus: error %d", status);
      assert(0);
      return;
    }
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
}

void openglCreateScreenVertArray(GLuint* buffer)
{
    GLfloat fbo_vertices[] = {
            -1, -1,
            1, -1,
            -1,  1,
            1,  1,
        };
    glGenBuffers(1, buffer);
    glBindBuffer(GL_ARRAY_BUFFER, *buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(fbo_vertices), fbo_vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void opengl_LoadTexture(LoadedTexture* tex)
{
    glGenTextures(1, (GLuint*)&tex->textureHandle);
    glBindTexture(GL_TEXTURE_2D, (GLuint)tex->textureHandle);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB8, tex->width, tex->height, 0, GL_RGB,
                  GL_UNSIGNED_BYTE, tex->data);
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    tex->boundToGPU = true;
}

void openglDeleteFbo(OpenglFrameBuffer *framebuffer)
{
    glDeleteFramebuffers(1, &framebuffer->fboHandle);
    glDeleteTextures(framebuffer->textureCount, framebuffer->fboTextures);
}

void openglCreateLightBuffers(u32 *workGroupsX, u32 *workGroupsY, u32 *lightBufferObject, u32 *lightElementBuffer, u32 width, u32 height)
{
    *workGroupsX = (width + (width % FPLUS_TILESIZE)) / FPLUS_TILESIZE;
    *workGroupsY = (height + (height % FPLUS_TILESIZE)) / FPLUS_TILESIZE;
    size_t numberOfTiles = (*workGroupsX)*(*workGroupsY);
    // Generate our shader storage buffers
    glGenBuffers(1, lightBufferObject);
    glGenBuffers(1, lightElementBuffer);
    // Bind light buffer
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, *lightBufferObject);
    glBufferData(GL_SHADER_STORAGE_BUFFER, NUM_LIGHTS * sizeof(PointLight), 0, GL_DYNAMIC_DRAW);
    // Bind visible light indices buffer
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, *lightElementBuffer);
    glBufferData(GL_SHADER_STORAGE_BUFFER, numberOfTiles * sizeof(i32) * NUM_LIGHTS, 0, GL_STATIC_DRAW);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void openglDeleteLightBuffers(u32 lightBufferObject, u32 lightElementBuffer)
{
    glDeleteBuffers(1, &lightBufferObject);
    glDeleteBuffers(1, &lightElementBuffer);
}

void glinitLights(OpenglState *state)
{
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, state->lightBuffer);
    PointLight *pointLights = (PointLight*)glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_READ_WRITE);

    for (int i = 0; i < NUM_LIGHTS/16; i++) {
        PointLight *light = &pointLights[i];
        light->position = vec4((random()%10000)/100.f,0.5f,(random()%10000)/100.f, 1.0f);
        light->color = vec4(1.0f -(random()%1000)/1000.f, 1.0f - (random()%1000)/1000.f, 1.0f - (random()%1000)/1000.f, 0.5f);
        light->paddingAndRadius = vec4FromVec3AndW(vec3(0.0f,0.0f,0.0f), 15.f); // TODO: add light radius
    }
    glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void openglInit(OpenglState* state, u32 width, u32 height)
{
    // TODO: free shaders
    initializeComputeProgram(&state->lightCullShader, "shaders/forwardp_lightcull.glsl", ST_LightCull);
    initializeProgram(&state->depthOnlyShader,"shaders/vert_shadow_pass.glsl", "shaders/frag_shadow_pass.glsl", 0, ST_Surface);
    initializeProgram(&state->postProcForwardShader,"shaders/postproc_vert_forward.glsl", "shaders/postproc_frag_forward.glsl", 0, ST_PostProc);
    openglCreateDepthFBO(&state->zprepass_fbo, width, height, false); // TODO: delete at end
    openglCreateForwardFBO(&state->render_fbo, width, height);
    // TODO: delete and end
    openglCreateLightBuffers(&state->lightCullWorkGroupsX,&state->lightCullWorkGroupsY, &state->lightBuffer, &state->lightIndicesBuffer, width, height);
    glinitLights(state);

    openglCreateScreenVertArray(&state->screenVertBuffer);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);

    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);
    glDepthFunc(GL_LEQUAL);
    glDepthRange(0.0f, 1.0f);
    glEnable(GL_FRAMEBUFFER_SRGB);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    state->screenWidth = width;
    state->screenHeight = height;

    state->initialized = 1;

    state->night = 0;
}

void openglFreeResources(OpenglState* state)
{
    openglDeleteFbo(&state->zprepass_fbo);
    openglDeleteLightBuffers(state->lightBuffer, state->lightIndicesBuffer);
    openglDeleteFbo(&state->render_fbo);
}

r32 openglGetScreenDepth(OpenglState* glstate, Vec2 screenCoord)
{
    u32 x = (u32)((screenCoord.x/2.0f+0.5f) * (float)glstate->screenWidth);
    u32 y = (u32)((screenCoord.y/2.0f+0.5f) * (float)glstate->screenHeight);

    r32 ret;
    glBindFramebuffer(GL_FRAMEBUFFER, glstate->render_fbo.fboHandle);
    glReadPixels(x,y,1,1, GL_DEPTH_COMPONENT, GL_FLOAT, &ret);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    return ret;
}

r32 openglGetDepth(OpenglState* glstate, u32 x, u32 y)
{
    r32 ret;
    glBindFramebuffer(GL_FRAMEBUFFER, glstate->render_fbo.fboHandle);
    glReadPixels(x,y,1,1, GL_DEPTH_COMPONENT, GL_FLOAT, &ret);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    return ret;
}

void openglResize(OpenglState* state, u32 newWidth, u32 newHeight)
{
    if(state->initialized && (state->screenWidth != newWidth || state->screenHeight != newHeight))
    {
        printf("resize %d %d\n",newWidth,newHeight);
        openglDeleteFbo(&state->zprepass_fbo);
        openglDeleteLightBuffers(state->lightBuffer, state->lightIndicesBuffer);
        openglDeleteFbo(&state->render_fbo);

        openglCreateDepthFBO(&state->zprepass_fbo, newWidth, newHeight, false);
        openglCreateLightBuffers(&state->lightCullWorkGroupsX,&state->lightCullWorkGroupsY, &state->lightBuffer, &state->lightIndicesBuffer, newWidth, newHeight);
        openglCreateForwardFBO(&state->render_fbo, newWidth, newHeight);
        glinitLights(state);

        state->screenWidth = newWidth;
        state->screenHeight = newHeight;
    }
}

void openGLRenderCommands(OpenglState* glstate, RenderCommands *commands, u32 windowWidth, u32 windowHeight)
{
    Mat4 modelMatrix;
    GLuint prog;

    void* current = commands->pushBufferBase;
    Camera* cam = commands->camera;
    //u32 cmdcount = commands->commands;

    /*glDisable(GL_BLEND);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, glstate->zprepass_fbo.fboHandle);
    glClearDepth(1.0f);
    glClear(GL_DEPTH_BUFFER_BIT);

    // Z PREPASS
    glUseProgram(glstate->depthOnlyShader.program);
    glUniformMatrix4fv(glstate->depthOnlyShader.surface.perspectiveMatrixUnif, 1, GL_FALSE, (const GLfloat*) &cam->perspectiveMatrix);

    while(commands->commands > 0)
    {
        RenderGroupEntryHeader *header = (RenderGroupEntryHeader *)current;
        current = (u8 *) current + header->size;
        //void *data = (u8 *) header + sizeof(RenderGroupEntryHeader);
        switch(header->type)
        {
            case RenderGroupEntryType_Mesh:
            {
                MeshEntry *entry = (MeshEntry *)data;
                modelMatrix = calculateModelMatrix(entry->transform);
                mat4Mul(&modelMatrix, &cam->transformMatrix, &modelMatrix);
                glUniformMatrix4fv(glstate->depthOnlyShader.surface.transformMatrixUnif, 1, GL_FALSE, (const GLfloat*) &modelMatrix);
                renderMesh(entry->mesh);
            } break;
            case RenderGroupEntryType_ArrayMesh:
            {
                TerrainMeshEntry *entry = (TerrainMeshEntry *)data;
                glUniformMatrix4fv(glstate->depthOnlyShader.surface.transformMatrixUnif, 1, GL_FALSE, (const GLfloat*) &cam->transformMatrix);
                glEnableVertexAttribArray(0);
                glBindBuffer(GL_ARRAY_BUFFER, entry->mesh->AttribBuffer);
                glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, entry->mesh->vertexStride, (void*)0);
                glDrawArrays(GL_TRIANGLES, 0, entry->mesh->vertices); // TODO: according to docs this causes sync problems with feedback buffer
                glBindBuffer(GL_ARRAY_BUFFER, 0);
            } break;
        }
        commands->commands--;
    }
    commands->commands = cmdcount;
    current = commands->pushBufferBase;

    glUseProgram(0);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);*/

    // LIGHT CULLING

    glUseProgram(glstate->lightCullShader.program);
    glUniformMatrix4fv(glstate->lightCullShader.lightc.projection, 1, GL_FALSE, (const GLfloat*) &cam->perspectiveMatrix);
    glUniformMatrix4fv(glstate->lightCullShader.lightc.view, 1, GL_FALSE, (const GLfloat*) &cam->transformMatrix);

    glUniform1i(glstate->lightCullShader.lightc.lightCount, NUM_LIGHTS);
    glUniform2i(glstate->lightCullShader.lightc.screenSize, glstate->screenWidth, glstate->screenHeight);

    glActiveTexture(GL_TEXTURE4);
    glUniform1i(glstate->lightCullShader.lightc.depthMap, 4);
    // using depth texture from last frame
    glBindTexture(GL_TEXTURE_2D, glstate->render_fbo.forwardFBO.depthTexture);

    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, glstate->lightBuffer);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, glstate->lightIndicesBuffer);
    glDispatchCompute(glstate->lightCullWorkGroupsX, glstate->lightCullWorkGroupsY, 1);

    glBindTexture(GL_TEXTURE_2D, 0);
    glUseProgram(0);

    // RENDER

    glBindFramebuffer(GL_FRAMEBUFFER, glstate->render_fbo.fboHandle);
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glClearDepth(1.0f);
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, glstate->lightBuffer);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, glstate->lightIndicesBuffer);

    //glstate->
    Vec4 ldir;
    if(glstate->night)
    {
        ldir = vec4(0.0f,0.316f,0.9486f, 0.0f);
    }
    else
    {
        ldir = vec4(0.0f,0.316f,0.9486f, 0.87f);
    }

    u32 renderedEntities = 0;

    while(commands->commands > 0)
    {
        renderedEntities++;
        RenderGroupEntryHeader *header = (RenderGroupEntryHeader *)current;
        current = (u8 *) current + header->size;
        void *data = (u8 *) header + sizeof(RenderGroupEntryHeader);
        switch(header->type)
        {
            case RenderGroupEntryType_Mesh:
            {
                MeshEntry *entry = (MeshEntry *)data;
                Material* material = &entry->material;
                prog = material->shader->program;
                //if(prog != prevProgram) // TODO: implement
                {
                    glUseProgram(material->shader->program);

                    if(material->shader->type == ST_Surface)
                    {
                        glUniformMatrix4fv(material->shader->surface.viewMatixUnif, 1, GL_FALSE, (const GLfloat*) &cam->transformMatrix);
                    }
                }
                modelMatrix = calculateModelMatrix(entry->transform);
                glUniformMatrix4fv(material->shader->surface.modelMatrix, 1, GL_FALSE, (const GLfloat*) &modelMatrix);
                mat4Mul(&modelMatrix, &cam->transformMatrix, &modelMatrix);
                glUniformMatrix4fv(material->shader->surface.transformMatrixUnif, 1, GL_FALSE, (const GLfloat*) &modelMatrix);
                glUniform3fv(material->shader->surface.cameraPosition, 1, (const GLfloat*) &cam->position);
                glUniform1i(material->shader->surface.numberOfTilesX, ceil(glstate->screenWidth/(float)FPLUS_TILESIZE));
                glUniform4fv(material->shader->surface.lightDirUnif, 1, (const GLfloat*)&ldir);

                glUniform1i(material->shader->surface.diffuseTexture, 0);
                glUniform1i(material->shader->surface.normalTexture, 1);

                for(int textureId = 0; textureId < material->numTextures; textureId++)
                {
                    glActiveTexture(GL_TEXTURE0+textureId);
                    glBindTexture(GL_TEXTURE_2D, (GLuint)material->texture_handle[textureId]);
                }
                renderMesh(entry->mesh);
                //prevProgram = prog;
            } break;
        case RenderGroupEntryType_ArrayMesh:
            {
                TerrainMeshEntry *entry = (TerrainMeshEntry *)data;
                Material* material = &entry->material;
                prog = material->shader->program;

                glUseProgram(prog);

                if(material->shader->surface.perspectiveMatrixUnif == -1 || material->shader->surface.viewMatixUnif == -1)
                {
                    __builtin_trap();
                }

                Mat4 mat = calculateModelMatrix(entry->transform);
                mat4Mul(&mat, &cam->transformMatrix, &mat);

                glUniformMatrix4fv(material->shader->surface.perspectiveMatrixUnif, 1, GL_FALSE, (const GLfloat*)&cam->perspectiveMatrix);
                glUniformMatrix4fv(material->shader->surface.viewMatixUnif, 1, GL_FALSE, (const GLfloat*)&mat);
                glUniform3fv(material->shader->surface.cameraPosition, 1, (const GLfloat*) &cam->position);
                glUniform1i(material->shader->surface.numberOfTilesX, ceil(glstate->screenWidth/(float)FPLUS_TILESIZE));

                glUniform4fv(material->shader->surface.lightDirUnif, 1, (const GLfloat*)&ldir);

                /*glBindBuffer(GL_ARRAY_BUFFER, entry->mesh->AttribBuffer);
                glEnableVertexAttribArray(0);
                glEnableVertexAttribArray(1);
                glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, entry->mesh->vertexStride, (void*)0);
                glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, entry->mesh->vertexStride, (void*)16);
                glDrawArrays(GL_TRIANGLES, 0, entry->mesh->vertices);
                glDisableVertexAttribArray(0);
                glDisableVertexAttribArray(1);*/

                //printf("render terrain \n");

                glBindVertexArray(entry->mesh->VAO);
                glDrawElements(GL_TRIANGLES, entry->mesh->faces*3, GL_UNSIGNED_INT, 0);
                glBindVertexArray(0);

            } break;
        default:
            {
                printf("Invalid render command! id: %d\n",header->type);
            } break;
        }

        commands->commands--;
    }
    //printf("entities rendered: %d\n", renderedEntities);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glClearColor(1.0, 0.0, 0.0, 0.0);
    glClearDepth(1.0f);
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

    glBindBuffer(GL_ARRAY_BUFFER, glstate->screenVertBuffer);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);

    glUseProgram(glstate->postProcForwardShader.program);
    glUniform1i(glstate->postProcForwardShader.postproc.albedo_unif, 0);
    glUniform1i(glstate->postProcForwardShader.postproc.depth_unif,  1);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, glstate->render_fbo.forwardFBO.diffuseTexture);
    glActiveTexture(GL_TEXTURE0+1);
    glBindTexture(GL_TEXTURE_2D, glstate->render_fbo.forwardFBO.depthTexture);
    glEnableVertexAttribArray(0);

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

}
