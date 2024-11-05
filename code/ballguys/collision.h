
#ifndef _COLLISION_H_
#define _COLLISION_H_

#include "vec3d.h"

#include "jsdf_array.h"

typedef struct AABB
{
    Vec3d min;
    Vec3d max;
} AABB;

typedef enum
{
    COLLIDER_PLANE,
    COLLIDER_SPHERE,
    COLLIDER_CYLINDER,
    COLLIDER_BOX
} ColliderType;

// an oriented, infinite plane
typedef struct
{
    Vec3d normal;
} Plane;

typedef struct
{
    float radius;
} Sphere;

typedef struct
{
    float radius;
    float height;
} Cylinder;

typedef struct
{
    float width;
    float height;
    float depth;
} Box;

typedef union
{
    Sphere sphere;
    Cylinder cylinder;
    Box box;
    Plane plane;
} ColliderShape;

typedef struct
{
    ColliderType type;
    Vec3d position;
    ColliderShape shape;
} Collider;

jsdf_array(Collider);

typedef struct
{
    Vec3d normal;
    float penetration;
} CollisionResult;

int Collision_intersectAABBAABB(AABB *a, AABB *b);

int Collision_testSegmentAABBCollision(Vec3d *p0, Vec3d *p1, AABB *b);

int Collision_testSphereColliders(
    Vec3d *sphereCenter,
    float sphereRadius,
    ColliderArray *colliders,
    CollisionResult *result);
#endif /* !_COLLISION_H_ */
