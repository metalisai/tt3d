#include "debug.h"

#include <GL/glew.h>
#include "lib/parson.h"

// TODO: get rid of malloc
void* lineMem = malloc(5000000);
int lines = 0;
GLuint lineBuffer;
GLuint lineVAO;
void setupDebug()
{
    glGenBuffers(1, &lineBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, lineBuffer);
    glBufferData(GL_ARRAY_BUFFER, MAX_DEBUG_LINES*sizeof(Vec4), lineMem, GL_DYNAMIC_DRAW);
    glGenVertexArrays(1, &lineVAO);
    glBindVertexArray(lineVAO);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void drawLine(Vec3 start, Vec3 end)
{
    Vec4* lineData = (Vec4*)lineMem;
    lineData[lines*2] = Vec4(start,1.0f);
    lineData[lines*2+1] = Vec4(end,1.0f);
    lines++;
}

void drawDebugLines(Shader lineShader, Camera* cam)
{
    glUseProgram(lineShader.program);
    glUniformMatrix4fv(lineShader.surface.transformMatrixUnif, 1, GL_FALSE, (GLfloat*)&cam->transformMatrix);
    glUniformMatrix4fv(lineShader.surface.perspectiveMatrixUnif, 1, GL_FALSE, (GLfloat*)&cam->perspectiveMatrix);

    glBindBuffer(GL_ARRAY_BUFFER, lineBuffer);
    glBindVertexArray(lineVAO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, lines*2*sizeof(Vec4), lineMem);

    glEnableVertexAttribArray(0); //?
    // TODO: why the fuck this cause some objects to not render??
    glDrawArrays(GL_LINES, 0, lines*2);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glUseProgram(0);
    lines= 0;
}
