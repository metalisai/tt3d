#include "engine.h"
#include "core.h"

void openglCreateDepthFBO(OpenglFrameBuffer* framebuffer, int width, int height, b32 isShadowMap)
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

void openglDeleteFbo(OpenglFrameBuffer* framebuffer)
{
    glDeleteFramebuffers(1, &framebuffer->fboHandle);
    glDeleteTextures(framebuffer->textureCount, framebuffer->fboTextures);
}

#define FPLUS_TILESIZE 16
#define NUM_LIGHTS 1024
void openglCreateLightBuffers(u32* workGroupsX, u32* workGroupsY, u32* lightBufferObject, u32* lightElementBuffer, u32 width, u32 height)
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
