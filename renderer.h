#ifndef RENDERER_H
#define RENDERER_H

#include <GL/glew.h>
#include "shared.h"

#include <string.h>

#define MAX_TEXTURES 2
#define FPLUS_TILESIZE 16
#define NUM_LIGHTS 1024

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
    GLuint numberOfTilesX;
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
    GLuint worldOffset;
    GLuint voxelScale;
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
    void* data;
    int width;
    int height;
    b32 used;
    b32 boundToGPU;
    void* textureHandle;
} LoadedTexture;

typedef struct TexturePool
{
    LoadedTexture *beginAddr;
    int size;
} TexturePool;

typedef struct ShaderProperty
{
    char* name;
    void* handle;
} ShaderProperty;

typedef struct Material
{
    Shader *shader;
    void* texture_handle[MAX_TEXTURES];
    u32 numTextures;
    ShaderProperty properties[10];
    u32 numProperties;
} Material;

typedef struct Mesh
{
    u32 faces;
    GLuint VAO;
    GLuint AttribBuffer;
    GLuint ElementBuffer;
    u32 vertices;
    u32 normals;
    b32 loadedToGPU;
    r32 boundingRadius;
    void *data;
} Mesh;

typedef struct ArrayMesh
{
    int faces;
    GLuint AttribBuffer;
    u32 vertices;
    b32 loadedToGPU;
    r32 boundingRadius;
    u32 vertexStride;
    void *data;
} ArrayMesh;

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
    union
    {
        Mesh mesh;
        ArrayMesh amesh;
    };
    Material material;
    b32 visible;
    u32 entityType;
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

TexturePool *createTexturePool(i32 maxTextures, MemoryArena *stack);
LoadedTexture *loadTextureToMemory(TexturePool *pool, const char *filename);

GLuint createShader(GLenum eShaderType, const char *strShaderFile, int fsize);
GLuint loadShader(GLenum eShaderType, const char *strShaderFilename);
GLuint createProgram(GLuint *shaderList, int pcount);
void initializeProgram(Shader *shader, const char *vertFile, const char *fragFile, const char *geoFile, enum ShaderType type);
void initializeComputeProgram(Shader *shader, const char *computeFile, enum ShaderType type);
void deleteProgram(Shader *shader);

void meshInit(Mesh *mesh);
void openGL_loadMesh(Mesh *mesh);
void renderMesh(Mesh *mesh);
void meshRecalculateNormals(Mesh *mesh);
void meshRecalculateBounds(Mesh *mesh);

void transformInit(Transform *transform);
void setPosition(Transform *transform, Vec3 pos);
void updateTranslationMatrix(Transform *transform);
void updateScaleMatrix(Transform *transform);
void updateRotationMatrix(Transform *transform);
Mat4 calculateModelMatrix(Transform *transform);

void cameraRecalculateMatrices(Camera *camera);
Vec3 cameraCalculateForwardDirection(Camera *camera);
Vec3 cameraCalculateUpDirection(Camera *camera);
Vec3 cameraCalculateRightDirection(Camera *camera);
Mat4 cameraCalculateInverseViewMatrix(Camera *camera);
void cameraInitialize(Camera *camera);
Vec3 cameraScreenToWorldPoint(Camera *cam, Vec2 screenPos);
Vec2 cameraWorldToScreenPoint(Camera *cam, Vec3 worldPos);
Vec3 cameraScreenDepthToWorldPoint(Camera *cam, Vec2 screenPos, r32 depth);
r32 cameraLinearizeDepth(Camera *cam, r32 depth);
b32 cameraIsPointInFrustum(Camera *cam, Vec3 worldPos);

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

typedef struct OpenglForwardFBOTextures
{
    u32 diffuseTexture;
    u32 depthTexture;
}OpenglForwardFBOTextures;

typedef struct OpenglFrameBuffer
{
    u32 fboHandle;
    union
    {
        u32 fboTextures[6];
        OpenglDepthFBOTextures depthFBO;
        OpenglDeferredFBOTextures deferredFBO;
        OpenglForwardFBOTextures forwardFBO;
    };
    u32 textureCount;
} OpenglFrameBuffer;

typedef struct OpenglState
{
    Shader depthOnlyShader;
    Shader lightCullShader;
    Shader postProcForwardShader;
    OpenglFrameBuffer zprepass_fbo;
    OpenglFrameBuffer render_fbo;
    GLuint screenVertBuffer;

    u32 lightCullWorkGroupsX;
    u32 lightCullWorkGroupsY;
    u32 lightBuffer;
    u32 lightIndicesBuffer;

    u32 screenWidth;
    u32 screenHeight;

    b32 initialized;
    b32 night;
} OpenglState;

void opengl_LoadTexture(LoadedTexture *tex);
void openglCreateDepthFBO(OpenglFrameBuffer *framebuffer, int width, int height, b32 isShadowMap);
void openglCreateDeferredFBO(OpenglFrameBuffer *framebuffer, u32 screenW, u32 screenH);
void openglDeleteFbo(OpenglFrameBuffer *framebuffer);
void openglDeleteZPassBuffer(u32 frameBufferHandle, u32 depthTextureHandle);
void openglCreateLightBuffers(u32 *workGroupsX, u32 *workGroupsY, u32 *lightBufferObject, u32 *lightElementBuffer, u32 width, u32 height);

#endif

typedef struct RenderCommands
{
    u32 width;
    u32 height;
    Camera* camera; // TODO: does this belong here?

    u8* pushBufferBase;
    u8* pushBufferDataAt;

    u32 maxPushBufferSize;
    u32 commands;

    Vec4 clearColor;
} RenderCommands;

typedef enum RenderGroupEntryType
{
    RenderGroupEntryType_Mesh,
    RenderGroupEntryType_ArrayMesh
} RenderGroupEntryType;

typedef struct RenderGroupEntryHeader
{
    u32 type;
    u32 size;
} RenderGroupEntryHeader;

typedef struct RenderGroup
{
    RenderCommands* commands;
} RenderGroup;

typedef struct MeshEntry
{
    Mesh* mesh;
    Transform* transform;
    Material material;
} MeshEntry;

typedef struct TerrainMeshEntry
{
    ArrayMesh* mesh;
    Transform* transform;
    Material material;
} TerrainMeshEntry;

void allocateRenderGroup(MemoryArena* arena, RenderGroup* renderGroup);
RenderGroupEntryHeader* pushBuffer(RenderGroup* renderGroup, u32 size);
void resetBuffer(RenderGroup* renderGroup);

void materialLoadProperties(Material* mat);

void pushMesh(RenderGroup *group, Mesh* mesh, Transform* transform, Material material);
void pushArrayMesh(RenderGroup *group, ArrayMesh* mesh, Transform* transform, Material material);
void* pushRenderElement(RenderGroup *group, u32 size, RenderGroupEntryType type);

void openglInit(OpenglState* state, u32 width, u32 height);
void openGLRenderCommands(OpenglState* state, RenderCommands *commands, u32 windowWidth, u32 windowHeight);
void openglFreeResources(OpenglState* state);
void openglResize(OpenglState* state, u32 newWidth, u32 newHeight);
GLuint opengl_Int16Texture2D(u32 width, u32 height, void* data);
GLuint opengl_Int16Texture1D(u32 width, void* data);
r32 openglGetScreenDepth(OpenglState* glstate, Vec2 screenCoord);
r32 openglGetDepth(OpenglState* glstate, u32 x, u32 y);

#endif // RENDERER_H
