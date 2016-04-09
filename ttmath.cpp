#include "ttMath.h"

#include <cmath>

IVec2::IVec2(){}
IVec2::IVec2(i32 x, i32 y)
{
    this->x = x;
    this->y = y;
}

Vec2::Vec2(){}
Vec2::Vec2(r32 x, r32 y)
{
    this->x = x;
    this->y = y;
}

Vec2 Vec2::operator-(const Vec2& b)
{
    Vec2 ret;
    ret.x = this->x - b.x;
    ret.y = this->y - b.y;
    return ret;
}

Vec3::Vec3(){}

Vec3::Vec3(r32 x, r32 y, r32 z)
{
    this->x = x;
    this->y = y;
    this->z = z;
}

Vec3::Vec3(Vec4 vec)
{
    this->x = vec.x;
    this->y = vec.y;
    this->z = vec.z;
}

Vec3 Vec3::operator*(const r32 b)
{
    Vec3 ret;
    ret.x = b*this->x;
    ret.y = b*this->y;
    ret.z = b*this->z;
    return ret;
}

Vec3 Vec3::operator+(const Vec3& b)
{
    Vec3 ret;
    ret.x = this->x + b.x;
    ret.y = this->y + b.y;
    ret.z = this->z + b.z;
    return ret;
}

Vec3 Vec3::operator-(const Vec3& b)
{
    Vec3 ret;
    ret.x = this->x - b.x;
    ret.y = this->y - b.y;
    ret.z = this->z - b.z;
    return ret;
}

Vec3 Vec3::operator/(const r32 b)
{
    Vec3 ret;
    ret.x = this->x/b;
    ret.y = this->y/b;
    ret.z = this->z/b;
    return ret;
}

Vec3& Vec3::operator+=(const Vec3& b)
{
    this->x += b.x;
    this->y += b.y;
    this->z += b.z;
    return *this;
}

Vec3& Vec3::operator-=(const Vec3& b)
{
    this->x -= b.x;
    this->y -= b.y;
    this->z -= b.z;
    return *this;
}


r32 Vec3::length()
{
    return sqrt(this->x*this->x+this->y*this->y+this->z*this->z);
}

Vec3 Vec3::normalized()
{
    return *this/this->length();
}

Vec3 Vec3::cross(const Vec3& b)
{
    Vec3 ret;
    ret.x = this->y*b.z-this->z*b.y;
    ret.y = this->z*b.x-this->x*b.z;
    ret.z = this->x*b.y-this->y*b.x;
    return ret;
}

r32 Vec3::dot(const Vec3& b)
{
    return this->x*b.x+this->y*b.y+this->z*b.z;
}

Vec4::Vec4() {}

Vec4::Vec4(r32 x, r32 y, r32 z, r32 w)
{
    this->x = x;
    this->y = y;
    this->z = z;
    this->w = w;
}

Vec4::Vec4(Vec3 xyz, r32 w)
{
    this->x = xyz.x;
    this->y = xyz.y;
    this->z = xyz.z;
    this->w = w;
}

Vec4 Vec4::operator/(const r32 b)
{
    Vec4 ret;
    ret.x = this->x/b;
    ret.y = this->y/b;
    ret.z = this->z/b;
    ret.w = this->w/b;
    return ret;
}

Vec4 Vec4::operator+(const Vec4& b)
{
    Vec4 ret;
    ret.x = this->x + b.x;
    ret.y = this->y + b.y;
    ret.z = this->z + b.z;
    ret.w = this->w + b.w;
    return ret;
}

Vec4 Vec4::operator-(const Vec4& b)
{
    Vec4 ret;
    ret.x = this->x - b.x;
    ret.y = this->y - b.y;
    ret.z = this->z - b.z;
    ret.w = this->w - b.w;
    return ret;
}

Quaternion::Quaternion()
{
}

Quaternion::Quaternion(r32 w, r32 x, r32 y, r32 z)
{
    this->w = w;
    this->x = x;
    this->y = y;
    this->z = z;
}

/*Quaternion::Quaternion(Quaternion& quat)
{
    this->w = quat.w;
    this->x = quat.x;
    this->y = quat.y;
    this->z = quat.z;
}*/

Quaternion::Quaternion(Vec3 a, r32 angle)
{
    this->x = a.x * sin(angle/2);
    this->y = a.y * sin(angle/2);
    this->z = a.z * sin(angle/2);
    this->w = cos(angle/2);
}

Quaternion Quaternion::operator*(const Quaternion& b)
{
    Quaternion ret;

    ret.w = this->w * b.w - this->x * b.x - this->y * b.y - this->z * b.z;  // 1
    ret.x = this->w * b.x + this->x * b.w + this->y * b.z - this->z * b.y;  // i
    ret.y = this->w * b.y - this->x * b.z + this->y * b.w + this->z * b.x;  // j
    ret.z = this->w * b.z + this->x * b.y - this->y * b.x + this->z * b.w;  // k

    return ret;
}

Mat4 Quaternion::toMat4()
{
    Mat4 ret;
    ret.m[0] = 1 - 2*this->y*this->y - 2*this->z*this->z;
    ret.m[1] = 2*this->x*this->y + 2*this->z*this->w;
    ret.m[2] = 2*this->x*this->z - 2*this->y*this->w;
    ret.m[3] = 0.0f;

    ret.m[4] = 2*this->x*this->y - 2*this->z*this->w;
    ret.m[5] = 1 - 2*this->x*this->x - 2*this->z*this->z;
    ret.m[6] = 2*this->y*this->z + 2*this->x*this->w;
    ret.m[7] = 0.0f;

    ret.m[8] = 2*this->x*this->z + 2*this->y*this->w;
    ret.m[9] = 2*this->y*this->z - 2*this->x*this->w;
    ret.m[10] = 1 - 2*this->x*this->x - 2*this->y*this->y;
    ret.m[11] = 0.0f;

    ret.m[12] = 0.0f;
    ret.m[13] = 0.0f;
    ret.m[14] = 0.0f;
    ret.m[15] = 1.0f;
    return ret;
}

Quaternion conjugate(Quaternion q)
{
    return Quaternion(-q.x,-q.y,-q.z,q.w);
}

Mat4::Mat4() { }

Mat4& Mat4::operator *= (Mat4 const & in)
{
    // TODO: is this intermediate step necessary?
    float i11, i12, i13, i14;
    float i21, i22, i23, i24;
    float i31, i32, i33, i34;
    float i41, i42, i43, i44;

    i11 = m11 * in.m11 + m12 * in.m21 + m13 * in.m31 + m14 * in.m41;
    i12 = m11 * in.m12 + m12 * in.m22 + m13 * in.m32 + m14 * in.m42;
    i13 = m11 * in.m13 + m12 * in.m23 + m13 * in.m33 + m14 * in.m43;
    i14 = m11 * in.m14 + m12 * in.m24 + m13 * in.m34 + m14 * in.m44;
    i21 = m21 * in.m11 + m22 * in.m21 + m23 * in.m31 + m24 * in.m41;
    i22 = m21 * in.m12 + m22 * in.m22 + m23 * in.m32 + m24 * in.m42;
    i23 = m21 * in.m13 + m22 * in.m23 + m23 * in.m33 + m24 * in.m43;
    i24 = m21 * in.m14 + m22 * in.m24 + m23 * in.m34 + m24 * in.m44;
    i31 = m31 * in.m11 + m32 * in.m21 + m33 * in.m31 + m34 * in.m41;
    i32 = m31 * in.m12 + m32 * in.m22 + m33 * in.m32 + m34 * in.m42;
    i33 = m31 * in.m13 + m32 * in.m23 + m33 * in.m33 + m34 * in.m43;
    i34 = m31 * in.m14 + m32 * in.m24 + m33 * in.m34 + m34 * in.m44;
    i41 = m41 * in.m11 + m42 * in.m21 + m43 * in.m31 + m44 * in.m41;
    i42 = m41 * in.m12 + m42 * in.m22 + m43 * in.m32 + m44 * in.m42;
    i43 = m41 * in.m13 + m42 * in.m23 + m43 * in.m33 + m44 * in.m43;
    i44 = m41 * in.m14 + m42 * in.m24 + m43 * in.m34 + m44 * in.m44;

    m11 = i11; m12 = i12; m13 = i13; m14 = i14;
    m21 = i21; m22 = i22; m23 = i23; m24 = i24;
    m31 = i31; m32 = i32; m33 = i33; m34 = i34;
    m41 = i41; m42 = i42; m43 = i43; m44 = i44;

    return *this;
}

//#define _USE_SIMD
Mat4 Mat4::operator * (Mat4 const & in)
{
    Mat4 ret;
    // TODO: BROKEN
    // NB! NB! NB! NB! NB! NB! NB! NB! NB! NB! NB! NB! NB! NB! NB! NB!
    // SIMD BROKEN
#ifdef _USE_SIMD

#error I AM BROKEN
     const __m128 a = in.m1;
     const __m128 b = in.m2;
     const __m128 c = in.m3;
     const __m128 d = in.m4;

     __m128 t1, t2;

     t1 = _mm_set1_ps(m11);
     t2 = _mm_mul_ps(a, t1);
     t1 =_mm_set1_ps(m21);
     t2 = _mm_add_ps(_mm_mul_ps(b, t1), t2);
     t1 =_mm_set1_ps(m31);
     t2 = _mm_add_ps(_mm_mul_ps(c, t1), t2);
     t1 =_mm_set1_ps(m41);
     t2 = _mm_add_ps(_mm_mul_ps(d, t1), t2);

     _mm_store_ps(&ret.m[0], t2);

     t1 = _mm_set1_ps(m12);
     t2 = _mm_mul_ps(a, t1);
     t1 =_mm_set1_ps(m22);
     t2 = _mm_add_ps(_mm_mul_ps(b, t1), t2);
     t1 =_mm_set1_ps(m32);
     t2 = _mm_add_ps(_mm_mul_ps(c, t1), t2);
     t1 =_mm_set1_ps(m42);
     t2 = _mm_add_ps(_mm_mul_ps(d, t1), t2);

     _mm_store_ps(&ret.m[4], t2);

     t1 = _mm_set1_ps(m13);
     t2 = _mm_mul_ps(a, t1);
     t1 =_mm_set1_ps(m23);
     t2 = _mm_add_ps(_mm_mul_ps(b, t1), t2);
     t1 =_mm_set1_ps(m33);
     t2 = _mm_add_ps(_mm_mul_ps(c, t1), t2);
     t1 =_mm_set1_ps(m43);
     t2 = _mm_add_ps(_mm_mul_ps(d, t1), t2);

     _mm_store_ps(&ret.m[8], t2);

     t1 = _mm_set1_ps(m14);
     t2 = _mm_mul_ps(a, t1);
     t1 =_mm_set1_ps(m24);
     t2 = _mm_add_ps(_mm_mul_ps(b, t1), t2);
     t1 =_mm_set1_ps(m34);
     t2 = _mm_add_ps(_mm_mul_ps(c, t1), t2);
     t1 =_mm_set1_ps(m44);
     t2 = _mm_add_ps(_mm_mul_ps(d, t1), t2);

     _mm_store_ps(&ret.m[12], t2);

#else
    ret.m11 = m11 * in.m11 + m12 * in.m21 + m13 * in.m31 + m14 * in.m41;
    ret.m12 = m11 * in.m12 + m12 * in.m22 + m13 * in.m32 + m14 * in.m42;
    ret.m13 = m11 * in.m13 + m12 * in.m23 + m13 * in.m33 + m14 * in.m43;
    ret.m14 = m11 * in.m14 + m12 * in.m24 + m13 * in.m34 + m14 * in.m44;
    ret.m21 = m21 * in.m11 + m22 * in.m21 + m23 * in.m31 + m24 * in.m41;
    ret.m22 = m21 * in.m12 + m22 * in.m22 + m23 * in.m32 + m24 * in.m42;
    ret.m23 = m21 * in.m13 + m22 * in.m23 + m23 * in.m33 + m24 * in.m43;
    ret.m24 = m21 * in.m14 + m22 * in.m24 + m23 * in.m34 + m24 * in.m44;
    ret.m31 = m31 * in.m11 + m32 * in.m21 + m33 * in.m31 + m34 * in.m41;
    ret.m32 = m31 * in.m12 + m32 * in.m22 + m33 * in.m32 + m34 * in.m42;
    ret.m33 = m31 * in.m13 + m32 * in.m23 + m33 * in.m33 + m34 * in.m43;
    ret.m34 = m31 * in.m14 + m32 * in.m24 + m33 * in.m34 + m34 * in.m44;
    ret.m41 = m41 * in.m11 + m42 * in.m21 + m43 * in.m31 + m44 * in.m41;
    ret.m42 = m41 * in.m12 + m42 * in.m22 + m43 * in.m32 + m44 * in.m42;
    ret.m43 = m41 * in.m13 + m42 * in.m23 + m43 * in.m33 + m44 * in.m43;
    ret.m44 = m41 * in.m14 + m42 * in.m24 + m43 * in.m34 + m44 * in.m44;
#endif
    return ret;
}

Vec4 Mat4::operator * (Vec4 const & b)
{
    Vec4 ret;
    ret.x = b.x*this->m11+b.y*this->m12+b.z*this->m13+b.w*this->m14;
    ret.y = b.x*this->m21+b.y*this->m22+b.z*this->m23+b.w*this->m24;
    ret.z = b.x*this->m31+b.y*this->m32+b.z*this->m33+b.w*this->m34;
    ret.w = b.x*this->m41+b.y*this->m42+b.z*this->m43+b.w*this->m44;
    return ret;
}

Vec4 Mat4::getRow(int rowIndex)
{
    Vec4 ret;
    switch(rowIndex)
    {
    case 0:
        ret.x = this->m11;
        ret.y = this->m12;
        ret.z = this->m13;
        ret.w = this->m14;
        break;
    case 1:
        ret.x = this->m21;
        ret.y = this->m22;
        ret.z = this->m23;
        ret.w = this->m24;
        break;
    case 2:
        ret.x = this->m31;
        ret.y = this->m32;
        ret.z = this->m33;
        ret.w = this->m34;
        break;
    case 3:
        ret.x = this->m41;
        ret.y = this->m42;
        ret.z = this->m43;
        ret.w = this->m44;
        break;
    }
    return ret;
}

Plane::Plane() {}
Plane::Plane(Vec4 vec)
{
    this->a = vec.x;
    this->b = vec.y;
    this->c = vec.z;
    this->d = vec.w;
}

r32 min(r32 a, r32 b)
{
    return a < b ? a : b;
}

r32 max(r32 a, r32 b)
{
    return a > b ? a : b;
}

Mat4 projMatrix(float fov, float aspect, float zNear, float zFar)
{
    Mat4 ret;
    const float h = 1.0f/tan(fov*PI_OVER_360);
    float neg_depth = zNear-zFar;

    ret.m[0] = h / aspect;
    ret.m[1] = 0;
    ret.m[2] = 0;
    ret.m[3] = 0;

    ret.m[4] = 0;
    ret.m[5] = h;
    ret.m[6] = 0;
    ret.m[7] = 0;

    ret.m[8] = 0;
    ret.m[9] = 0;
    ret.m[10] = (zFar + zNear)/neg_depth;
    ret.m[11] = -1;

    ret.m[12] = 0;
    ret.m[13] = 0;
    ret.m[14] = 2.0f*(zNear*zFar)/neg_depth;
    ret.m[15] = 0;

    return ret;
}

Mat4 invPerspective(Mat4& perspectiveMat)
{
    Mat4 ret;
    float a = perspectiveMat.m[0];
    float b = perspectiveMat.m[5];
    float c = perspectiveMat.m[10];
    float d = perspectiveMat.m[14];
    float e = perspectiveMat.m[11];

    for(int i = 0; i < 16; i++)
        ret.m[i]  = 0.0f;

    ret.m[0]  = 1.0f / a;
    ret.m[5]  = 1.0f / b;
    ret.m[11] = 1.0f / d;
    ret.m[14] = 1.0f / e;
    ret.m[15] = -c / (d * e);

    return ret;
}

Mat4 ortho(r32 left, r32 right, r32 bottom, r32 top, r32 near, r32 far)
{
    Mat4 ret;

    ret.m11 = 2.f/(right-left);
    ret.m12 = 0.f;
    ret.m13 = 0.f;
    ret.m14 = -(right+left)/(right-left);

    ret.m21 = 0.f;
    ret.m22 = 2.f/(top-bottom);
    ret.m23 = 0.f;
    ret.m24 = -(top+bottom)/(top-bottom);

    ret.m31 = 0.f;
    ret.m32 = 0.f;
    ret.m33 = -2.f/(far-near);
    ret.m34 = -(far+near)/(far-near);

    ret.m41 = 0.f;
    ret.m42 = 0.f;
    ret.m43 = 0.f;
    ret.m44 = 1.0f;

    return ret;
}

Mat4 scale(Vec3 scale)
{
    Mat4 ret;
    ret.m[0] = scale.x;
    ret.m[1] = 0.0f;
    ret.m[2] = 0.0f;
    ret.m[3] = 0.0f;

    ret.m[4] = 0.0f;
    ret.m[5] = scale.y;
    ret.m[6] = 0.0f;
    ret.m[7] = 0.0f;

    ret.m[8] = 0.0f;
    ret.m[9] = 0.0f;
    ret.m[10] = scale.z;
    ret.m[11] = 0.0f;

    ret.m[12] = 0.0f;
    ret.m[13] = 0.0f;
    ret.m[14] = 0.0f;
    ret.m[15] = 1.0f;
    return ret;
}

Mat4 translate(Vec3 pos)
{
    Mat4 ret;
    ret.m[0] = 1.0f;
    ret.m[1] = 0.0f;
    ret.m[2] = 0.0f;
    ret.m[3] = 0.0f;

    ret.m[4] = 0.0f;
    ret.m[5] = 1.0f;
    ret.m[6] = 0.0f;
    ret.m[7] = 0.0f;

    ret.m[8] = 0.0f;
    ret.m[9] = 0.0f;
    ret.m[10] = 1.0f;
    ret.m[11] = 0.0f;

    ret.m[12] = pos.x;
    ret.m[13] = pos.y;
    ret.m[14] = pos.z;
    ret.m[15] = 1.0f;
    return ret;
}

void getFrustumPlanes(Mat4& projectionMatrix, Plane* planeArray)
{
    planeArray[0] = Plane(projectionMatrix.getRow(3)-projectionMatrix.getRow(0));
    planeArray[1] = Plane(projectionMatrix.getRow(3)+projectionMatrix.getRow(0));
    planeArray[2] = Plane(projectionMatrix.getRow(3)-projectionMatrix.getRow(1));
    planeArray[3] = Plane(projectionMatrix.getRow(3)+projectionMatrix.getRow(1));
    planeArray[4] = Plane(projectionMatrix.getRow(3)-projectionMatrix.getRow(2));
    planeArray[5] = Plane(projectionMatrix.getRow(3)+projectionMatrix.getRow(2));
}
