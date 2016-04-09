#ifndef SHADER_H
#define SHADER_H

#include <GL/glew.h>
#include <string>
#include <vector>

struct SurfaceShader
{
    GLuint lightDirUnif;
    GLuint transformMatrixUnif;
    GLuint perspectiveMatrixUnif;
    GLuint viewMatixUnif;
    GLuint diffuseTexture;
    GLuint normalTexture;
};

struct SurfaceShaderForw : SurfaceShader
{
    GLuint lightDirUnif;
    GLuint transformMatrixUnif;
};

struct PostProcShader
{
    GLuint albedo_unif;
    GLuint depth_unif;
    GLuint normal_unif;
    GLuint position_unif;
    GLuint shadow_unif;
    GLuint light_unif;
    GLuint projM_unif;
    GLuint lightProjM_unif;
};

struct SkydomeShader
{
    GLuint scaleMatrix;
    GLuint perspectiveMatrix;           // perspective matrix
    GLuint transformMatrix;             // transform matrix
    GLuint v3CameraPos;                 // camera position in world coordinates
    GLuint v3LightDir;     				// Direction vector to the light source
    GLuint v3InvWavelength; 			// 1 / pow(wavelength, 4) for RGB
    GLuint fCameraHeight;   			// The camera's current heigh
    GLuint fCameraHeight2;   			// fCameraHeight^2
    GLuint fOuterRadius;     			// The outer (atmosphere) radius
    GLuint fOuterRadius2;    			// fOuterRadius^2
    GLuint fInnerRadius;     			// The inner (planetary) radius
    GLuint fInnerRadius2;    			// fInnerRadius^2
    GLuint fKrESun;          			// Kr * ESun
    GLuint fKmESun;          			// Km * ESun
    GLuint fKr4PI;           			// Kr * 4 * PI
    GLuint fKm4PI;           			// Km * 4 * PI
    GLuint fScale;           			// 1 / (fOuterRadius - fInnerRadius)
    GLuint fScaleOverScaleDepth;        // fScale / fScaleDepth
};

struct LightCullShader
{
    GLuint lightCount;
    GLuint screenSize;
    GLuint view;
    GLuint projection;
    GLuint depthMap;
};

enum ShaderType
{
    ST_Surface,
    ST_PostProc,
    ST_Skydome,
    ST_LightCull
};

struct Shader
{
    GLuint program;
    union
    {
        SurfaceShader surface;
        PostProcShader postproc;
        SkydomeShader skydome;
        LightCullShader lightc;
    };
    ShaderType type;
};

GLuint createShader(GLenum eShaderType, const std::string &strShaderFile);
GLuint loadShader(GLenum eShaderType, const std::string &strShaderFilename);
GLuint createProgram(const std::vector<GLuint> &shaderList);
void initializeProgram(Shader* shader, const char* vertFile, const char* fragFile, ShaderType type);
void initializeComputeProgram(Shader* shader, const char* computeFile, ShaderType type);
void deleteProgram(Shader* shader);

#endif // SHADER_H
