#ifndef DEBUG_H
#define DEBUG_H

#include "ttMath.h"
#include <GL/glew.h>
#include "shader.h"
#include "camera.h"

#define MAX_DEBUG_LINES 2048

void setupDebug();
void drawLine(Vec3 start, Vec3 end);
void drawDebugLines(Shader lineShader, Camera* cam);

#endif // DEBUG_H
