#include "camera.h"

#include <math.h>

void updateCameraMatrices(Camera* camera)
{
    camera->perspectiveMatrix = projMatrix(camera->FOV, camera->aspectRatio, camera->nearPlane, camera->farPlane);
    Mat4 translation = translate(camera->position*-1.f);
    Mat4 rotato = Quaternion(-camera->rotation.w,camera->rotation.x,camera->rotation.y,camera->rotation.z).toMat4();;
    camera->transformMatrix = rotato*translation;
}

Vec3 Camera::forwardDir()
{
    Vec4 res = Quaternion(this->rotation.w,this->rotation.x,this->rotation.y,this->rotation.z).toMat4()*Vec4(0.f,0.f,-1.f,0.f);
    return Vec3(res.x,res.y,res.z).normalized();
}

Vec3 Camera::upDir()
{
    Vec4 res = Quaternion(this->rotation.w,this->rotation.x,this->rotation.y,this->rotation.z).toMat4()*Vec4(0.f,1.f,0.f,0.f);
    return Vec3(res.x,res.y,res.z).normalized();
}

Vec3 Camera::rightDir()
{
    Vec4 res = Quaternion(this->rotation.w,this->rotation.x,this->rotation.y,this->rotation.z).toMat4()*Vec4(1.f,0.f,0.f,0.f);
    return Vec3(res.x,res.y,res.z).normalized();
}

Mat4 Camera::invViewMat()
{
    Mat4 translation = translate(this->position);
    Mat4 rotato = Quaternion(this->rotation.w,this->rotation.x,this->rotation.y,this->rotation.z).toMat4();;
    return translation*rotato;
}
