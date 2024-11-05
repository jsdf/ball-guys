#include <assert.h>
#include <math.h>
#include <stdlib.h>

#include "constants.h"
#include "macros.h"
#include "collision.h"
#include "vec3d.h"

jsdf_array_funcs(Collider, malloc, free);

void AABB_fromSphere(Vec3d *sphereCenter, float sphereRadius, AABB *result)
{
    result->min = *sphereCenter;
    result->min.x -= sphereRadius;
    result->min.y -= sphereRadius;
    result->min.z -= sphereRadius;

    result->max = *sphereCenter;
    result->max.x += sphereRadius;
    result->max.y += sphereRadius;
    result->max.z += sphereRadius;
}

void AABB_expandByPoint(AABB *self, Vec3d *point)
{
    self->min.x = MIN(self->min.x, point->x);
    self->min.y = MIN(self->min.y, point->y);
    self->min.z = MIN(self->min.z, point->z);

    self->max.x = MAX(self->max.x, point->x);
    self->max.y = MAX(self->max.y, point->y);
    self->max.z = MAX(self->max.z, point->z);
}

int Collision_intersectAABBAABB(AABB *a, AABB *b)
{
    // Exit with no intersection if separated along an axis
    if (a->max.x < b->min.x || a->min.x > b->max.x)
        return FALSE;
    if (a->max.y < b->min.y || a->min.y > b->max.y)
        return FALSE;
    if (a->max.z < b->min.z || a->min.z > b->max.z)
        return FALSE; // Overlapping on all axes means AABBs are intersecting
    return TRUE;
}

float Collision_sqDistancePointAABB(Vec3d *p, AABB *b)
{
    float v, dist;
    float sqDist = 0.0f;
    // For each axis count any excess distance outside box extents
    v = p->x;
    if (v < b->min.x)
    {
        dist = (b->min.x - v);
        sqDist += dist * dist;
    }
    if (v > b->max.x)
    {
        dist = (v - b->max.x);
        sqDist += dist * dist;
    }

    v = p->y;
    if (v < b->min.y)
    {
        dist = (b->min.y - v);
        sqDist += dist * dist;
    }
    if (v > b->max.y)
    {
        dist = (v - b->max.y);
        sqDist += dist * dist;
    }

    v = p->z;
    if (v < b->min.z)
    {
        dist = (b->min.z - v);
        sqDist += dist * dist;
    }
    if (v > b->max.z)
    {
        dist = (v - b->max.z);
        sqDist += dist * dist;
    }
    return sqDist;
}

// Returns true if sphere intersects AABB, false otherwise
int Collision_testSphereAABBCollision(Vec3d *sphereCenter,
                                      float sphereRadius,
                                      AABB *aabb)
{
    // Compute squared distance between sphere center and AABB
    float sqDist = Collision_sqDistancePointAABB(sphereCenter, aabb);
    // Sphere and AABB intersect if the (squared) distance
    // between them is less
    // than the (squared) sphere radius
    return sqDist <= sphereRadius * sphereRadius;
}

// Test if segment specified by points p0 and p1 intersects AABB b
// from Real Time Collision Detection ch5.3
int Collision_testSegmentAABBCollision(Vec3d *p0, Vec3d *p1, AABB *b)
{
    Vec3d c;
    Vec3d e;
    Vec3d m;
    Vec3d d;
    float adx;
    float ady;
    float adz;

    // Box center-point
    // c = (b->min + b->max) * 0.5f;
    c = b->min;
    Vec3d_add(&c, &b->max);
    Vec3d_mulScalar(&c, 0.5f);

    // Box halflength extents
    // e = b->max - c;
    e = b->max;
    Vec3d_sub(&e, &c);

    // Segment midpoint
    // (p0 + p1) * 0.5f;
    m = *p0;
    Vec3d_add(&m, p1);
    Vec3d_mulScalar(&m, 0.5f);

    // Segment halflength vector
    // d = p1 - m;
    d = *p1;
    Vec3d_sub(&d, &m);

    // Translate box and segment to origin
    // m = m - c;
    Vec3d_sub(&m, &c);

    // Try world coordinate axes as separating axes
    adx = fabsf(d.x);
    if (fabsf(m.x) > e.x + adx)
        return FALSE;
    ady = fabsf(d.y);
    if (fabsf(m.y) > e.y + ady)
        return FALSE;
    adz = fabsf(d.z);
    if (fabsf(m.z) > e.z + adz)
        return FALSE;
    // Add in an epsilon term to counteract arithmetic errors when segment is
    // (near) parallel to a coordinate axis (see text for detail)
    adx += FLT_EPSILON;
    ady += FLT_EPSILON;
    adz += FLT_EPSILON;
    // Try cross products of segment direction vector with coordinate axes
    if (fabsf(m.y * d.z - m.z * d.y) > e.y * adz + e.z * ady)
        return FALSE;
    if (fabsf(m.z * d.x - m.x * d.z) > e.x * adz + e.z * adx)
        return FALSE;
    if (fabsf(m.x * d.y - m.y * d.x) > e.x * ady + e.y * adx)
        return FALSE; // No separating axis found; segment must be overlapping AABB
    return TRUE;
}

int Collision_testSphereSphereCollision(
    Vec3d *sphereCenter1,
    float sphereRadius1,
    Vec3d *sphereCenter2,
    float sphereRadius2,
    CollisionResult *result)
{
    float distSquared = Vec3d_distanceToSq(sphereCenter1, sphereCenter2);
    float totalRadius = sphereRadius1 + sphereRadius2;
    if (distSquared > totalRadius * totalRadius)
    {
        return FALSE;
    }
    result->penetration = totalRadius - sqrtf(distSquared);
    Vec3d direction = *sphereCenter2;
    Vec3d_sub(&direction, sphereCenter1);
    Vec3d_normalise(&direction);
    Vec3d_mulScalar(&direction, result->penetration);
    result->normal = direction;
    return TRUE;
}

int Collision_testSpherePlaneCollision(
    Vec3d *sphereCenter,
    float sphereRadius,
    Vec3d *planeNormal,
    Vec3d *planePoint,
    CollisionResult *result)
{
    // distance from sphere center to plane
    float dist = Vec3d_dot(planeNormal, sphereCenter) - Vec3d_dot(planeNormal, planePoint);
    // if the distance is greater than the sphere radius, there is no overlap
    if (dist > sphereRadius)
    {
        return FALSE;
    }
    // the remaining distance is the penetration depth of the sphere into the plane
    result->penetration = sphereRadius - dist;
    // set the normal to the direction of penetration, which is the
    // opposite of the plane normal
    Vec3d_copyFrom(&result->normal, planeNormal);
    Vec3d_mulScalar(&result->normal, -1.0f);

    return TRUE;
}

int Collision_testSphereCollider(
    Vec3d *sphereCenter,
    float sphereRadius,
    Collider *collider,
    Vec3d *colliderPosition,
    CollisionResult *result)
{
    switch (collider->type)
    {

    case COLLIDER_PLANE:
        return Collision_testSpherePlaneCollision(sphereCenter, sphereRadius,
                                                  &collider->shape.plane.normal,
                                                  colliderPosition, result);

    case COLLIDER_SPHERE:
        return Collision_testSphereSphereCollision(sphereCenter, sphereRadius,
                                                   colliderPosition,
                                                   collider->shape.sphere.radius,
                                                   result);

    case COLLIDER_CYLINDER:
        die("cylinder collision not implemented");

    case COLLIDER_BOX:
        die("box collision not implemented");

    default:
        die("unknown collider type");
    }
}

int Collision_testSphereColliders(
    Vec3d *sphereCenter,
    float sphereRadius,
    ColliderArray *colliders,
    CollisionResult *result)
{
    for (size_t i = 0; i < colliders->length; i++)
    {
        Collider *collider = ColliderArray_at(colliders, i);
        if (Collision_testSphereCollider(
                sphereCenter,
                sphereRadius,
                collider,
                &collider->position,
                result))
        {
            return TRUE;
        }
    }
    return FALSE;
}
