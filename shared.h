#ifndef SHARED_H
#define SHARED_H

#ifdef __cplusplus
extern "C" {
#endif

#define USE_SIMD 1

#ifdef USE_SIMD
#include <immintrin.h>
#include <emmintrin.h>
#endif

#include <stdint.h>
#include <float.h>
#include <stdio.h>

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

typedef i32 b32;

typedef float r32;
typedef double r64;

#define true    1
#define false   0

// #define ENGINEBUILD_SLOW

#define ARRAY_COUNT( array ) (sizeof( array ) / (sizeof( array[0] )))

#define Kilobytes(size) ((size)*1024LL)
#define Megabytes(size) (Kilobytes(size)*1024LL)

#ifdef ENGINEBUILD_SLOW
#define assert(expr) {if(!(expr)) { printf("Assertion failed at %s:%d in function %s\n",__FILE__,__LINE__,__func__); __builtin_trap(); }}
#else
#define assert(expr)
#endif

#define is_aligned(POINTER, BYTE_COUNT) \
    (((uintptr_t)(const void *)(POINTER)) % (BYTE_COUNT) == 0)

#define MEMORYARENA_ALIGNMENT 16

typedef struct MemoryArena
{
    char* start;
    char* pointer;
    u64 size;
    u64 align; // 16 byte alignment padding
} MemoryArena;

#define PI 3.14159265
#define PI_OVER_360 0.00872664625


#define VEC2_OP(a,b,op,c) \
    a->x = b->x op c->x; \
    a->y = b->y op c->y;

#define VEC3_OP(a,b,op,c) \
    a->x = b->x op c->x; \
    a->y = b->y op c->y; \
    a->z = b->z op c->z;

#define VEC4_OP(a,b,op,c) \
    a->x = b->x op c->x; \
    a->y = b->y op c->y; \
    a->z = b->z op c->z; \
    a->w = b->w op c->w;

#define INVALID_CODE_PATH {__builtin_trap();}
#define INT2VOIDP(i) (void*)(uintptr_t)(i)

#define U16MAX 65535
#define U32MAX ((u32)-1)
#define R32MAX FLT_MAX
#define R32MIN -FLT_MAX

struct Vec4;

typedef struct IVec2
{
    i32 x;
    i32 y;
} IVec2;

typedef struct Vec2
{
    r32 x;
    r32 y;
} Vec2;

typedef struct IVec3
{
    i32 x;
    i32 y;
    i32 z;
} IVec3;

typedef struct Vec3
{
    r32 x;
    r32 y;
    r32 z;
} Vec3;

typedef struct Vec4
{
    r32 x;
    r32 y;
    r32 z;
    r32 w;
} Vec4;

typedef struct Quaternion
{
    r32 w;
    r32 x;
    r32 y;
    r32 z;
} Quaternion;

// COLUMN MAJOR
typedef struct Mat4
{
    union
    {
           struct
           {
                  __m128 m1, m2, m3, m4;
           };

           struct
           {
                  float m[16];
           };

           struct
           {
                float m11, m21, m31, m41;
                float m12, m22, m32, m42;
                float m13, m23, m33, m43;
                float m14, m24, m34, m44;
                /*
                float m11, m12, m13, m14;
                float m21, m22, m23, m24;
                float m31, m32, m33, m34;
                float m41, m42, m43, m44;*/
           };

           struct
           {
                  Vec4 v[4];
           };
    };
} Mat4;

typedef struct Plane
{
    r32 a;
    r32 b;
    r32 c;
    r32 d;
} Plane;

#define KEY_COUNT   32
typedef struct Input
{
    u8 keyStates[KEY_COUNT];
    u8 keyStatesLastFrame[KEY_COUNT];
    Vec2 mousePosition;
    Vec2 mouseCoord;
} Input;

void* arenaPushSize(MemoryArena *stack, u64 size);
void createArena(MemoryArena *stack, void* start, u64 size);

int charToKeycode(char ch);

Quaternion conjugate(Quaternion q);
r32 min(r32 a, r32 b);
r32 max(r32 a, r32 b);
Mat4 projMatrix(float fov, float aspect, float zNear, float zFar);
Mat4 ortho(r32 left, r32 right, r32 bottom, r32 top, r32 near, r32 far);
Mat4 invPerspective(Mat4 *perspectiveMat);
Mat4 scale(Vec3 scale);
Mat4 translate(Vec3 pos);
void getFrustumPlanes(Mat4 *projectionMatrix, Plane* planeArray);

IVec2 ivec2(i32 x, i32 y);
Vec2 vec2(r32 x, r32 y);
Vec3 vec3(r32 x, r32 y, r32 z);
Vec3 vec3FromVec4(Vec4 v);
Vec4 vec4(r32 x, r32 y, r32 z, r32 w);
Vec4 vec4FromVec3AndW(Vec3 v, r32 w);

Vec2 vec2Zero();
Vec3 vec3Zero();
Vec4 vec4Zero();

Quaternion quaternion(r32 w, r32 x, r32 y, r32 z);
Quaternion quaternionFromAxisAngle(Vec3 axis, r32 angle);

r32 vec2Mag(Vec2 *v);

void vec2Sub(Vec2 *d, Vec2 *v0, Vec2 *v1);
void vec3Sub(Vec3 *d, Vec3 *v0, Vec3* v1);
void vec4Sub(Vec4 *d, Vec4 *v0, Vec4 *v1);
void vec2Add(Vec2 *d, Vec2 *v0, Vec2 *v1);
void vec3Add(Vec3 *d, Vec3 *v0, Vec3 *v1);
void vec4Add(Vec4 *d, Vec4 *v0, Vec4 *v1);
void vec2Mul(Vec2 *d, Vec2 *v0, Vec2 *v1);
void vec3Mul(Vec3 *d, Vec3 *v0, Vec3 *v1);
void vec4Mul(Vec4 *d, Vec4 *v0, Vec4 *v1);
void vec3Scale(Vec3 *d, Vec3 *v, r32 s);
void vec4Scale(Vec4 *d, Vec4 *v, r32 s);

void quaternionMul(Quaternion *d, Quaternion *q0, Quaternion *q1);

void mat4FromQuaternion(Mat4 *d, Quaternion *quat);
void mat4Mul(Mat4 *d, Mat4 *m0, Mat4 *m1);
void mat4Vec4Mul(Vec4 *d, Mat4 *m, Vec4 *v);
Vec4 mat4GetRow(Mat4 *m, int rowIndex);

r32 vec3Dot(Vec3 *v0, Vec3 *v1);
Vec3 vec3Cross(Vec3 *v0, Vec3 *v1);
Vec3 vec3Normalized(Vec3 *v);
r32 vec3Mag(Vec3 *v);
r32 vec3Mag2(Vec3 *v);
r32 clamp01(r32 v);

Plane planeFromVec4(Vec4 *v);
r32 floorf(r32 x);
r32 powf(r32 x, r32 power);
r32 absf(r32 x);
r32 sqrtf(r32 x);
b32 isnanf(r32 x);

#ifdef __cplusplus
}
#endif

#endif // SHARED_H
