#include <t3d/t3d.h>
#include <t3d/t3dmodel.h>
#include "drawobj.h"
#include "game.h"
#include "physics.h"
#include "physobj.h"
#include "player.h"
#include "vec3d.h"
#include "camera.h"

CameraFocus Camera_getPlayersBounds()
{
    // Step 1: Calculate the center of the bounding sphere
    Vec3d center;
    Vec3d_origin(&center);

    for (int i = 0; i < MAXPLAYERS; i++)
    {
        Player *player = Player_getPlayer(i);
        PhysBody *physBody = PhysObj_getByID(player->objID);
        Vec3d_add(&center, &physBody->position);
    }
    Vec3d_divScalar(&center, (float)MAXPLAYERS); // Average position

    // Step 2: Calculate the radius of the bounding sphere
    float maxDistance = 0.0f;
    for (int i = 0; i < MAXPLAYERS; i++)
    {
        Player *player = Player_getPlayer(i);
        PhysBody *physBody = PhysObj_getByID(player->objID);
        float distance = Vec3d_distanceTo(&center, &physBody->position);
        if (distance > maxDistance)
        {
            maxDistance = distance;
        }
    }

    CameraFocus focus;
    Vec3d_copyFrom(&focus.center, &center);
    focus.radius = maxDistance;

    return focus;
}

void Camera_update(Camera *camera,
                   float deltaTime)
{
    CameraFocus focus = Camera_getPlayersBounds();

    // Step 3: Calculate the required camera distance
    float verticalDistance = focus.radius / sinf(camera->fov / 2.0f);
    float horizontalFov = 2.0f * atanf(tanf(camera->fov / 2.0f) * camera->aspectRatio);
    float horizontalDistance = focus.radius / sinf(horizontalFov / 2.0f);
    float distance = fmaxf(verticalDistance, horizontalDistance);

    // Step 4: Calculate the target camera position
    Vec3d targetPosition;
    Vec3d_copyFrom(&targetPosition, &focus.center);
    Vec3d tempDirection;
    Vec3d_mulScalar(Vec3d_copyFrom(&tempDirection, &camera->direction), -distance);
    Vec3d_add(&targetPosition, &tempDirection);

    float smoothFactor = 0.1f;
    // Step 5: Smoothly interpolate the camera position
    Vec3d_lerp(&camera->position, &targetPosition, deltaTime * smoothFactor);
}
