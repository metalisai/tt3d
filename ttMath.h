#ifndef TTMATH_H
#define TTMATH_H

#include "types.h"
#include <emmintrin.h>

// TODO: keep double?
#define PI 3.14159265
#define PI_OVER_360 0.00872664625

struct Vec4;

struct IVec2
{
    i32 x;
    i32 y;

    IVec2();
    IVec2(i32 x, i32 y);
};

struct Vec2
{
    r32 x;
    r32 y;

    Vec2();
    Vec2(r32 x, r32 y);
    Vec2 operator-(const Vec2& b);
};

struct Vec3
{
    r32 x;
    r32 y;
    r32 z;

    Vec3();
    Vec3(r32 x, r32 y, r32 z);
    //Vec3(glm::vec3);
    Vec3(Vec4 vec);
    //void operator=(const glm::vec3& b);
    Vec3 operator+(const Vec3& b);
    Vec3 operator-(const Vec3& b);
    //Vec3 operator+(const glm::vec3& b);
    Vec3 operator*(const r32 b);
    Vec3 operator/(const r32 b);
    Vec3& operator+=(const Vec3& b);
    Vec3& operator-=(const Vec3& b);
    //Vec3& operator+=(const glm::vec3& b);
    Vec3 cross(const Vec3& b);
    r32 dot(const Vec3& b);
    r32 length();
    Vec3 normalized();
};

struct Vec4
{
    r32 x;
    r32 y;
    r32 z;
    r32 w;

    Vec4();
    Vec4(r32 x, r32 y, r32 z, r32 w);
    Vec4(Vec3 xyz, r32 w);
    Vec4 operator/(const r32 b);
    Vec4 operator+(const Vec4& b);
    Vec4 operator-(const Vec4& b);
};

struct Quaternion
{
    r32 w;
    r32 x;
    r32 y;
    r32 z;

    Quaternion();
    Quaternion(r32 w, r32 x, r32 y, r32 z);
    //Quaternion(Quaternion& quat);
    Quaternion(Vec3 a, r32 angle);
    Quaternion operator*(const Quaternion& b);
    struct Mat4 toMat4();
};

// COLUMN MAJOR
struct Mat4
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

    Mat4();
    Mat4& operator *= (Mat4 const & in);
    Mat4 operator * (Mat4 const & in);
    Vec4 operator * (Vec4 const & b);
    Vec4 getRow(int rowIndex);
};

struct Plane
{
    r32 a;
    r32 b;
    r32 c;
    r32 d;

    Plane();
    Plane(Vec4 vec);
};

Quaternion conjugate(Quaternion q);
r32 min(r32 a, r32 b);
r32 max(r32 a, r32 b);
Mat4 projMatrix(float fov, float aspect, float zNear, float zFar);
Mat4 ortho(r32 left, r32 right, r32 bottom, r32 top, r32 near, r32 far);
Mat4 invPerspective(Mat4& perspectiveMat);
Mat4 scale(Vec3 scale);
Mat4 translate(Vec3 pos);
void getFrustumPlanes(Mat4& projectionMatrix, Plane* planeArray);

#endif // TTMATH_H
