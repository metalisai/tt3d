#include "renderer.h"

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

Vec3 cameraScreenToWorldPoint(Camera *cam, Vec2 screenPos)
{
    Vec4 screenCoord = vec4(screenPos.x, screenPos.y, -1.0f, 1.0);

    Mat4 mat = cameraCalculateInverseViewMatrix(cam);
    Mat4 mat2 = invPerspective(&cam->perspectiveMatrix);
    Mat4 camInvPerspective;
    mat4Mul(&camInvPerspective, &mat, &mat2);

    Vec4 ret4;
    mat4Vec4Mul(&ret4, &camInvPerspective, &screenCoord);
    vec4Scale(&ret4, &ret4, 1.0/ret4.w);

    return vec3FromVec4(ret4);
}

r32 cameraLinearizeDepth(Camera *cam, r32 depth)
{
  float n = cam->nearPlane;
  float f = cam->farPlane;
  float z = depth;
  return (2.0f * n) / (f + n - z * (f - n));
}

Vec3 cameraScreenDepthToWorldPoint(Camera *cam, Vec2 screenPos, r32 depth)
{
    Vec4 screenCoord = vec4(screenPos.x, screenPos.y, depth, 1.0);

    Mat4 mat = cameraCalculateInverseViewMatrix(cam);
    Mat4 mat2 = invPerspective(&cam->perspectiveMatrix);
    Mat4 camInvPerspective;
    mat4Mul(&camInvPerspective, &mat, &mat2);

    Vec4 ret4;
    mat4Vec4Mul(&ret4, &camInvPerspective, &screenCoord);
    vec4Scale(&ret4, &ret4, 1.0/ret4.w);

    return vec3FromVec4(ret4);
}

Vec2 cameraWorldToScreenPoint(Camera *cam, Vec3 worldPos)
{
    Vec4 worldCoord = vec4FromVec3AndW(worldPos, 1.0);
    Mat4 viewProjection;
    mat4Mul(&viewProjection, &cam->perspectiveMatrix, &cam->transformMatrix);
    Vec4 ret4;
    mat4Vec4Mul(&ret4, &viewProjection, &worldCoord);
    vec4Scale(&ret4, &ret4, 1.0/ret4.w);
    Vec2 ret;
    ret.x = ret4.x;
    ret.y = ret4.y;
    return ret;
}

// TODO: alot of slow stuff in it
b32 cameraIsPointInFrustum(Camera *cam, Vec3 worldPos)
{
    Plane frustumPlanes[6];
    getFrustumPlanes(&cam->perspectiveMatrix, frustumPlanes);
    for(int i = 0; i < 6; i++)
    {
        Vec4 thing = vec4(frustumPlanes[i].a,frustumPlanes[i].b,frustumPlanes[i].c,0.0f);
        Mat4 mat = cameraCalculateInverseViewMatrix(cam);
        Vec4 res;
        mat4Vec4Mul(&res, &mat, &thing);
        frustumPlanes[i] = planeFromVec4(&res);
    }

    Vec3 n[6];
    r32 d[6];

    for(int planeId = 0; planeId < 6; planeId++)
    {
        Vec3 idk = vec3(frustumPlanes[planeId].a,frustumPlanes[planeId].b,frustumPlanes[planeId].c);
        n[planeId] = vec3Normalized(&idk);
        d[planeId] = frustumPlanes[planeId].d;
    }

        // TODO: check for all 6 planes?
    Vec3 pos;
    vec3Sub(&pos, &worldPos, &cam->position);
    r32 res = vec3Dot(&pos, &n[0]) + d[0] + 141.0f;
    r32 res2 = vec3Dot(&pos, &n[1]) + d[1] + 141.0f;
    if(res <= 0.f || res2 <= 0.f)
        return false;
    return true;
}
