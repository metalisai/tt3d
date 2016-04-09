#ifndef CAMERA_H
#define CAMERA_H

#include "transform.h"

void updateCameraMatrices(struct Camera* camera);

struct Camera
{
    Vec3 position = Vec3(0.0f,0.0f,0.0f);
    Quaternion rotation = Quaternion(1.0f,0.0f,0.0f,0.0f);
    Mat4 perspectiveMatrix;
    Mat4 transformMatrix;

    r32 FOV;
    r32 nearPlane;
    r32 farPlane;

    r32 aspectRatio = 1.0f;

    Camera()
    {
        updateCameraMatrices(this);
    }
    Vec3 forwardDir();
    Vec3 upDir();
    Vec3 rightDir();
    Mat4 invViewMat();
};

void updateCameraMatrices(Camera* camera);

#endif // CAMERA_H
