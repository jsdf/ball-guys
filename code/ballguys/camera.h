#ifndef CAMERA_H
#define CAMERA_H

#include "vec3d.h"

typedef struct Camera
{
    Vec3d position;
    Vec3d direction;
    float fov;
    float aspectRatio;
    float maxDistance;
} Camera;

typedef struct CameraFocus
{
    Vec3d center;
    float radius;
} CameraFocus;

CameraFocus Camera_getPlayersBounds();

void Camera_update(Camera *camera,
                   float deltaTime);

#endif /* !CAMERA_H */