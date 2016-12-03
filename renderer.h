#ifndef RENDERER_H
#define RENDERER_H

#include <GL/glew.h>
#include "shared.h"

#include <string.h>

#define MAX_TEXTURES 2

typedef struct SurfaceShader
{
    GLuint lightDirUnif;
    GLuint transformMatrixUnif;
    GLuint perspectiveMatrixUnif;
    GLuint viewMatixUnif;
    GLuint diffuseTexture;
    GLuint normalTexture;
    GLuint cameraPosition;
    GLuint modelMatrix;
} SurfaceShader;

typedef struct TerrainGenShader
{
    GLuint lightDirUnif;
    GLuint transformMatrixUnif;
    GLuint perspectiveMatrixUnif;
    GLuint viewMatixUnif;
    GLuint mcubesTexture1;
    GLuint mcubesTexture2;
    GLuint diffuseTexture;
    GLuint cameraPosition;
} TerrainGenShader;

typedef struct PostProcShader
{
    GLuint albedo_unif;
    GLuint depth_unif;
    GLuint normal_unif;
    GLuint position_unif;
    GLuint shadow_unif;
    GLuint light_unif;
    GLuint projM_unif;
    GLuint lightProjM_unif;
} PosstProcShader;

typedef struct SkydomeShader
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
} SkydomeShader;

typedef struct LightCullShader
{
    GLuint lightCount;
    GLuint screenSize;
    GLuint view;
    GLuint projection;
    GLuint depthMap;
} LightCullShader;

enum ShaderType
{
    ST_Surface,
    ST_PostProc,
    ST_Skydome,
    ST_LightCull,
    ST_Particle
};

typedef struct Shader
{
    GLuint program;
    union
    {
        SurfaceShader surface;
        struct PostProcShader postproc;
        SkydomeShader skydome;
        LightCullShader lightc;
        TerrainGenShader terrainGen;
    };
    enum ShaderType type;
} Shader;

typedef struct LoadedTexture
{
    GLuint gl_handle;
    int width;
    int height;
    b32 used;
    b32 boundToGPU;
    void* data;
} LoadedTexture;

typedef struct TexturePool
{
    LoadedTexture *beginAddr;
    int size;
} TexturePool;

typedef struct Material
{
    Shader *shader;
    GLuint texture_handle[MAX_TEXTURES];
    u16 numTextures;
} Material;

typedef struct MeshCold
{
    int vertices;
    int normals;
    b32 loadedToGPU;
    void *data;
} MeshCold;

typedef struct Mesh
{
    int faces;
    GLuint VAO;
    GLuint AttribBuffer;
    GLuint ElementBuffer;
    MeshCold *cold;

    r32 boundingRadius;
} Mesh;

typedef struct Transform
{
    Vec3 position;
    Vec3 scale;
    Quaternion rotation;
    Mat4 rotationMatrix;
    Mat4 translationMatrix;
    Mat4 scaleMatrix;
    Mat4 modelMatrix;

    b32 dirty;
} Transform;

typedef struct Entity
{
    Transform transform;
    Mesh mesh;
    Material material;
    b32 visible;
} Entity;

typedef struct Camera
{
    Vec3 position;
    Quaternion rotation;
    Mat4 perspectiveMatrix;
    Mat4 transformMatrix;
    r32 FOV;
    r32 nearPlane;
    r32 farPlane;
    r32 aspectRatio;
} Camera;

TexturePool *createTexturePool(i32 maxTextures, MemStack *stack);
LoadedTexture *loadTextureToMemory(TexturePool *pool, const char* filename);
void openGL_LoadTexture(LoadedTexture *tex);

GLuint createShader(GLenum eShaderType, const char *strShaderFile, int fsize);
GLuint loadShader(GLenum eShaderType, const char *strShaderFilename);
GLuint createProgram(GLuint *shaderList, int pcount);
void initializeProgram(Shader *shader, const char *vertFile, const char *fragFile, enum ShaderType type);
void initializeComputeProgram(Shader *shader, const char *computeFile, enum ShaderType type);
void deleteProgram(Shader *shader);

void meshInit(Mesh *mesh);
void openGL_loadMesh(Mesh *mesh);
void renderMesh(Mesh *mesh);
void meshRecalculateNormals(Mesh *mesh);
void meshRecalculateBounds(Mesh *mesh);

void transformInit(Transform* transform);
void setPosition(Transform *transform, Vec3 pos);
void updateTranslationMatrix(struct Transform *transform);
void updateScaleMatrix(struct Transform *transform);
void updateRotationMatrix(struct Transform *transform);
Mat4 calculateModelMatrix(Transform *transform);

void cameraRecalculateMatrices(struct Camera *camera);
Vec3 cameraCalculateForwardDirection(struct Camera *camera);
Vec3 cameraCalculateUpDirection(struct Camera *camera);
Vec3 cameraCalculateRightDirection(struct Camera *camera);
Mat4 cameraCalculateInverseViewMatrix(struct Camera *camera);
void cameraInitialize(struct Camera *camera);

// OPENGL
#define OPENGL_SUPPORTED 1

#ifdef OPENGL_SUPPORTED
typedef struct OpenglDepthFBOTextures
{
    u32 depthTexture;
} OpenglDepthFBOTextures;

typedef struct OpenglDeferredFBOTextures
{
    u32 diffuseTexture;
    u32 normalTexture;
    u32 positionTexture;
    u32 depthTexture;
}OpenglDeferredFBOTextures;

typedef struct OpenglFrameBuffer
{
    u32 fboHandle;
    union
    {
        u32 fboTextures[6];
        OpenglDepthFBOTextures depthFBO;
        OpenglDeferredFBOTextures deferredFBO;
    };
    u32 textureCount;
} OpenglFrameBuffer;

void openglCreateDepthFBO(OpenglFrameBuffer* framebuffer, int width, int height, b32 isShadowMap);
void openglCreateDeferredFBO(OpenglFrameBuffer *framebuffer, u32 screenW, u32 screenH);
void openglDeleteFbo(OpenglFrameBuffer* framebuffer);
void openglDeleteZPassBuffer(u32 frameBufferHandle, u32 depthTextureHandle);
void openglCreateLightBuffers(u32* workGroupsX, u32* workGroupsY, u32* lightBufferObject, u32* lightElementBuffer, u32 width, u32 height);
#endif

#endif // RENDERER_H
