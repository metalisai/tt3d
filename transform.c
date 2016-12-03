#include "renderer.h"

void transformInit(Transform* transform)
{
    transform->position = vec3(0.f,0.f,0.f);
    transform->scale = vec3(1.f,1.f,1.f);
    transform->rotation = quaternion(1.f,0.f,0.f,0.f);
    transform->dirty = true;

    updateTranslationMatrix(transform);
    updateScaleMatrix(transform);
    updateRotationMatrix(transform);
    //transform->modelMatrix = transform->translationMatrix*transform->rotationMatrix*transform->scaleMatrix;
}

void updateTranslationMatrix(Transform* transform)
{
    Mat4* mat = &transform->translationMatrix;
    mat->m[0] = 1.0f;
    mat->m[1] = 0.0f;
    mat->m[2] = 0.0f;
    mat->m[3] = 0.0f;

    mat->m[4] = 0.0f;
    mat->m[5] = 1.0f;
    mat->m[6] = 0.0f;
    mat->m[7] = 0.0f;

    mat->m[8] = 0.0f;
    mat->m[9] = 0.0f;
    mat->m[10] = 1.0f;
    mat->m[11] = 0.0f;

    mat->m[12] = transform->position.x;
    mat->m[13] = transform->position.y;
    mat->m[14] = transform->position.z;
    mat->m[15] = 1.0f;
}

void updateRotationMatrix(Transform* transform)
{
    mat4FromQuaternion(&transform->rotationMatrix, &transform->rotation);
}

void updateScaleMatrix(Transform* transform)
{
    Mat4* mat = &transform->scaleMatrix;
    mat->m[0] = transform->scale.x;
    mat->m[1] = 0.0f;
    mat->m[2] = 0.0f;
    mat->m[3] = 0.0f;

    mat->m[4] = 0.0f;
    mat->m[5] = transform->scale.y;
    mat->m[6] = 0.0f;
    mat->m[7] = 0.0f;

    mat->m[8] = 0.0f;
    mat->m[9] = 0.0f;
    mat->m[10] = transform->scale.z;
    mat->m[11] = 0.0f;

    mat->m[12] = 0.0f;
    mat->m[13] = 0.0f;
    mat->m[14] = 0.0f;
    mat->m[15] = 1.0f;
}

void setPosition(Transform* transform, Vec3 pos)
{
    transform->dirty = true;
    transform->position = pos;
}

Mat4 calculateModelMatrix(Transform* transform)
{
    if(transform->dirty)
    {
        updateTranslationMatrix(transform);
        updateScaleMatrix(transform);
        updateRotationMatrix(transform);
        Mat4 res;
        mat4Mul(&res, &transform->translationMatrix, &transform->rotationMatrix);
        mat4Mul(&transform->modelMatrix, &res, &transform->scaleMatrix);
        //transform->modelMatrix = transform->translationMatrix*transform->rotationMatrix*transform->scaleMatrix;
        transform->dirty = false;
        return transform->modelMatrix;
    }
    else
    {
        return transform->modelMatrix;
    }
}
