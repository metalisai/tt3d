#include "renderer.h"
#include <math.h>

void cameraRecalculateMatrices(Camera* camera)
{
    camera->perspectiveMatrix = projMatrix(camera->FOV, camera->aspectRatio, camera->nearPlane, camera->farPlane);
    Vec3 negCamPos;
    vec3Scale(&negCamPos, &camera->position, -1.0f);
    Mat4 translation = translate(negCamPos);
    Quaternion q = quaternion(-camera->rotation.w,camera->rotation.x,camera->rotation.y,camera->rotation.z);
    Mat4 rotato;
    mat4FromQuaternion(&rotato, &q);
    mat4Mul(&camera->transformMatrix, &rotato, &translation);
}

Vec3 cameraCalculateForwardDirection(Camera* camera)
{
    Quaternion q = quaternion(camera->rotation.w,camera->rotation.x,camera->rotation.y,camera->rotation.z);
    Mat4 m;
    mat4FromQuaternion(&m, &q);
    Vec4 dir = vec4(0.f,0.f,-1.f,0.f);
    Vec4 res;
    mat4Vec4Mul(&res, &m, &dir);
    Vec3 res3 = vec3(res.x,res.y,res.z);
    return vec3Normalized(&res3);
}

Vec3 cameraCalculateUpDirection(Camera* camera)
{
    Quaternion q = quaternion(camera->rotation.w,camera->rotation.x,camera->rotation.y,camera->rotation.z);
    Mat4 m;
    mat4FromQuaternion(&m, &q);
    Vec4 dir = vec4(0.f,1.f,0.f,0.f);
    Vec4 res;
    mat4Vec4Mul(&res, &m, &dir);
    Vec3 res3 = vec3(res.x,res.y,res.z);
    return vec3Normalized(&res3);
}

Vec3 cameraCalculateRightDirection(Camera* camera)
{
    Quaternion q = quaternion(camera->rotation.w,camera->rotation.x,camera->rotation.y,camera->rotation.z);
    Mat4 m;
    mat4FromQuaternion(&m, &q);
    Vec4 dir = vec4(1.f,0.f,0.f,0.f);
    Vec4 res;
    mat4Vec4Mul(&res, &m, &dir);
    Vec3 res3 = vec3(res.x,res.y,res.z);
    return vec3Normalized(&res3);
}

Mat4 cameraCalculateInverseViewMatrix(Camera* camera)
{
    Mat4 translation = translate(camera->position);
    Quaternion q = quaternion(camera->rotation.w,camera->rotation.x,camera->rotation.y,camera->rotation.z);
    Mat4 m;
    mat4FromQuaternion(&m, &q);
    // TODO: Remove
    Mat4 ret;
    mat4Mul(&ret, &translation, &m);
    return ret;
}

void cameraInitialize(Camera* camera)
{
    camera->position = vec3(0.0f,0.0f,0.0f);
    camera->rotation = quaternion(1.0f,0.0f,0.0f,0.0f);
    camera->aspectRatio = 1.0f;
    cameraRecalculateMatrices(camera);
}
