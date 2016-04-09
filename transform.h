#ifndef TRANSFORM_H
#define TRANSFORM_H

#include "ttMath.h"
#include "mesh.h"


struct Transform
{
    Vec3 position = Vec3(0.f,0.f,0.f);
    Vec3 scale = Vec3(1.f,1.f,1.f);
    Quaternion rotation = Quaternion(1.f,0.f,0.f,0.f);
    Mat4 rotationMatrix;
    Mat4 translationMatrix;
    Mat4 scaleMatrix;
    Mat4 modelMatrix;

    bool dirty = true;
    Transform();
};
void setPosition(Transform* transform, Vec3 pos);
void updateTranslationMatrix(struct Transform* transform);
void updateScaleMatrix(struct Transform* transform);
void updateRotationMatrix(struct Transform* transform);
Mat4* calculateModelMatrix(Transform* transform);

#endif // TRANSFORM_H
