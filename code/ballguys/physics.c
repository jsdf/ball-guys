
#include <math.h>

#include "collision.h"
#include "constants.h"
#include "macros.h"
#include "physics.h"
#include "vec3d.h"

#define PHYS_DEBUG 0
#define PHYS_MIN_MOVEMENT 0.5
#define PHYS_COLLISION_MIN_SEPARATION 0.001
#define PHYSICS_MOTION_DAMPENING 0
#define PHYSICS_USE_VERLET_INTEGRATION 0
#define PHYS_MAX_COLLISION_ITERATIONS 10
#define PHYS_DEBUG_PRINT_COLLISIONS 1

#define PHYS_BASE_VISCOSITY 0.05f

void PhysState_init(PhysState *self, PhysWorldData *worldData)
{
    self->dynamicTimestep = TRUE;
    self->worldData = worldData;
}

void PhysBody_init(PhysBody *self,
                   float mass,
                   float radius,
                   Vec3d *position,
                   int id)
{
    debugf("PhysBody_init %d\n", id);
    self->id = id;
    self->mass = mass;
    self->massInverse = 1.0 / mass;
    self->radius = radius;
    self->radiusSquared = radius * radius;
    self->restitution = 1.0;

    self->enabled = TRUE;
    self->position = *position;
    self->prevPosition = *position;
    self->flags = 0;
    self->velocity = (Vec3d){0, 0, 0};
    self->nonIntegralVelocity = (Vec3d){0, 0, 0};
    self->force = (Vec3d){0, 0, 0};

    self->nonIntegralAcceleration = (Vec3d){0, 0, 0};
    self->prevForce = (Vec3d){0, 0, 0};
}

void PhysBehavior_floorBounce(PhysBody *body, float floorHeight)
{
    float opposite;
    Vec3d response;

    opposite = (-1.0) * body->mass;
    if (body->position.y - body->radius < floorHeight)
    {
        body->position.y = floorHeight + body->radius;
        Vec3d_init(&response, 0.0, body->nonIntegralVelocity.y * opposite, 0.0);
        Vec3d_add(&body->force, &response);
    }
}

void PhysBehavior_floorClamp(PhysBody *body, float floorHeight)
{
    if (body->position.y - body->radius < floorHeight)
    {
        body->position.y = floorHeight + body->radius;
    }
}

int PhysBehavior_worldCollisionResponseStep(PhysBody *body,
                                            PhysWorldData *world)
{
    int hasCollision;

    Vec3d response, beforePos;
    CollisionResult collision;
    // increase the radius by the minimum separation to allow some slop
    hasCollision = Collision_testSphereColliders(
        &body->position, body->radius - PHYS_COLLISION_MIN_SEPARATION, &world->colliders, &collision);

    if (!hasCollision)
    {
        return FALSE;
    }

#ifdef PHYS_DEBUG_PRINT_COLLISIONS
    debugf("body %d collided\n", body->id);
#endif
    // response is to move away by penetration distance
    response = collision.normal;
    // move away by penetration distance (accounting for slop)
    Vec3d_mulScalar(&response, -1.0 * (collision.penetration + PHYS_COLLISION_MIN_SEPARATION));

    beforePos = body->position;

    // apply response
#if PHYSICS_USE_VERLET_INTEGRATION
    PhysBody_translateWithoutForce(body, &response);
#else
    PhysBody_translateWithoutForce(body, &response);
    Vec3d_origin(&body->nonIntegralVelocity); // stop movement

    // alternative: apply force
    // Vec3d_mulScalar(&response, collision.penetration * body->mass);
    // PhysBody_applyForce(body, &response);
#endif

#if PHYS_DEBUG_PRINT_COLLISIONS
    debugf(
        "--PhysBody id=%d hasCollision collision.penetration=%f collision.normal=",
        body->id, collision.penetration);
    Vec3d_print(&collision.normal, stderr);
    debugf("\n");

    debugf("beforePos=");
    Vec3d_print(&beforePos, stderr);
    debugf("\n");
    debugf("afterPos=");
    Vec3d_print(&body->position, stderr);
    debugf("\n");
    debugf("response=");
    Vec3d_print(&response, stderr);
    debugf("\n");
#endif

    return TRUE;
}

void PhysBehavior_collisionSeparationOffset(Vec3d *result,
                                            Vec3d *pos,
                                            float overlap,
                                            float separationForce)
{
    Vec3d_copyFrom(result, pos);
    Vec3d_normalise(result);
    Vec3d_mulScalar(result, overlap * separationForce);
}

int PhysBehavior_bodyBodyCollisionResponse(PhysBody *body,
                                           PhysBody *pool,
                                           int numInPool)
{
    Vec3d delta, direction, collisionSeparationOffset;
    int i, hasCollision;
    float distanceSquared, radii, distance, overlap, mt, bodySeparationForce,
        otherBodySeparationForce;
    PhysBody *otherBody;

    hasCollision = FALSE;

    Vec3d_origin(&delta);
    Vec3d_origin(&direction);

    for (i = 0, otherBody = pool; i < numInPool; i++, otherBody++)
    {
        if (body != otherBody && otherBody->enabled)
        {
            Vec3d_copyFrom(&delta, &otherBody->position);
            Vec3d_sub(&delta, &body->position);
            distanceSquared = Vec3d_magSq(&delta);
            Vec3d_copyFrom(&direction, &delta);
            Vec3d_normalise(&direction);
            radii = body->radius + otherBody->radius;
            if (distanceSquared <= radii * radii)
            { // collision
                hasCollision = TRUE;

                distance = sqrtf(distanceSquared);
                overlap = radii - distance /* - 0.5*/;
                /* Total mass. */
                mt = body->mass + otherBody->mass;
                /* Distribute collision responses. */
                bodySeparationForce = otherBody->mass / mt;
                otherBodySeparationForce = body->mass / mt;

                /* Move particles so they no longer overlap.*/
                PhysBehavior_collisionSeparationOffset(
                    &collisionSeparationOffset, &delta, overlap, -bodySeparationForce);
                Vec3d_add(&body->position, &collisionSeparationOffset);

                PhysBehavior_collisionSeparationOffset(&collisionSeparationOffset,
                                                       &delta, overlap,
                                                       otherBodySeparationForce);
                Vec3d_add(&otherBody->position, &collisionSeparationOffset);
            }
        }
    }
    return hasCollision;
}

int PhysBehavior_collisionResponseStep(PhysBody *body,
                                       PhysWorldData *world,
                                       PhysBody *pool,
                                       int numInPool)
{
    int hasCollision;

    hasCollision = FALSE;
    hasCollision =
        hasCollision || PhysBehavior_worldCollisionResponseStep(body, world);
    return hasCollision;
}

void PhysBehavior_collisionResponse(PhysWorldData *world,
                                    PhysBody *bodies,
                                    int numBodies)
{
    int i, k, hasAnyCollision;
    PhysBody *body;
    // int floorHeight = 0.0;
    for (k = 0, body = bodies; k < numBodies; k++, body++)
    {
        if (body->enabled)
        {
            // PhysBehavior_floorBounce(body, floorHeight);
            // PhysBehavior_floorClamp(body, floorHeight);
            PhysBehavior_bodyBodyCollisionResponse(body, bodies, numBodies);
        }
    }

    // run multiple iterations, because the response to a collision can create
    // another collision
    for (i = 0; i < PHYS_MAX_COLLISION_ITERATIONS; ++i)
    {
        hasAnyCollision = FALSE;
        for (k = 0, body = bodies; k < numBodies; k++, body++)
        {
            if (body->enabled)
            {
                hasAnyCollision =
                    hasAnyCollision ||
                    PhysBehavior_collisionResponseStep(body, world, bodies, numBodies);
            }
        }

        if (!hasAnyCollision)
        {
            break;
        }
    }
#ifdef PHYS_DEBUG
    if (i > 0)
    {
        debugf("collision response took %d iters\n", i);
    }
    if (hasAnyCollision)
    {
        debugf(
            "hit PHYS_MAX_COLLISION_ITERATIONS and ended collision response with "
            "collisions remaining\n");
    }
#endif // PHYS_DEBUG
}

void PhysBody_setEnabled(PhysBody *body, int enabled)
{
    if (enabled)
    {
        body->enabled = TRUE;
        // prevent velocity from movement while disabled
        Vec3d_copyFrom(&body->prevPosition, &body->position);
    }
    else
    {
        body->enabled = FALSE;
    }
}

void PhysBehavior_constantForce(PhysBody *body, Vec3d force)
{
    Vec3d_add(&body->force, &force);
}

void PhysBody_applyForce(PhysBody *body, Vec3d *force)
{
    Vec3d_add(&body->force, force);
}

// move but don't affect velocity
void PhysBody_translateWithoutForce(PhysBody *body, Vec3d *translation)
{
    Vec3d_add(&body->position, translation);
    Vec3d_add(&body->prevPosition, translation);
}

void PhysBody_update(PhysBody *self,
                     float dt,
                     PhysBody *pool,
                     int numInPool,
                     PhysState *physics)
{

    Vec3d gravityForce;
    Vec3d_init(&gravityForce, 0, physics->worldData->gravity * self->mass, 0);
    // do behaviours
    PhysBehavior_constantForce(self, gravityForce); // apply gravity
}

void PhysBody_dampenSmallMovements(PhysBody *body)
{
    // dampen small movements
    if (Vec3d_distanceTo(&body->position, &body->prevPosition) <
        PHYS_MIN_MOVEMENT)
    {
        body->position = body->prevPosition;
        Vec3d_origin(&body->velocity);
        Vec3d_origin(&body->nonIntegralVelocity);
        Vec3d_origin(&body->force);
        Vec3d_origin(&body->prevForce);
    }
}

void PhysBody_integrateMotionVerlet(PhysBody *body, float dt, float drag)
{
    Vec3d newPosition;
    Vec3d acceleration;
    Vec3d_origin(&newPosition);
    /* Scale force to mass. */
    acceleration = body->force;
    Vec3d_mulScalar(&acceleration, body->massInverse);
    /* Derive velocity. */
    Vec3d_copyFrom(&body->velocity, &body->position);
    Vec3d_sub(&body->velocity, &body->prevPosition);
    /* Apply friction. */
    Vec3d_mulScalar(&body->velocity, drag);
    /* Apply acceleration force to new position. */
    /* Get integral acceleration, apply to velocity, then apply updated
       velocity to position */
    Vec3d_copyFrom(&newPosition, &body->position);
    Vec3d_mulScalar(&acceleration, dt);
    Vec3d_add(&body->velocity, &acceleration);
    Vec3d_add(&newPosition, &body->velocity);

    /* Store old position, update position to new position. */
    Vec3d_copyFrom(&body->prevPosition, &body->position);
    Vec3d_copyFrom(&body->position, &newPosition);

#if PHYSICS_MOTION_DAMPENING
    PhysBody_dampenSmallMovements(&body);
#endif

    /* Reset acceleration force. */
    Vec3d_copyFrom(&body->prevForce, &body->force);
    Vec3d_origin(&body->force);

    /* store velocity for use in acc calculations by user code */
    Vec3d_copyFrom(&body->nonIntegralVelocity, &body->velocity);
    Vec3d_mulScalar(&body->nonIntegralVelocity, 1.0 / dt);
    body->nonIntegralAcceleration = acceleration; // store for debugging
    Vec3d_mulScalar(&body->nonIntegralAcceleration, 1.0 / dt);
}
void PhysBody_integrateMotionSemiImplicitEuler(PhysBody *body,
                                               float dt,
                                               float drag)
{
    // acceleration = force / mass
    // accelerationForDT = acceleration * dt
    // velocity = velocity + accelerationForDT
    // velocityForDT = velocity * dt
    // position = position + velocityForDT

    Vec3d newPosition;
    /* Scale force by mass to calculate actual acceleration */
    // acceleration = ( force / mass )
    Vec3d acceleration = body->force;

    Vec3d_mulScalar(&acceleration, body->massInverse);
    body->nonIntegralAcceleration = acceleration; // for debugging
    // accelerationForDT = acceleration * dt
    Vec3d_mulScalar(&acceleration, dt);

    // velocity = velocity + accelerationForDT
    Vec3d_add(&body->nonIntegralVelocity, &acceleration);
    Vec3d_mulScalar(&body->nonIntegralVelocity, drag);

    // velocityForDT = velocity * dt
    body->velocity = body->nonIntegralVelocity;
    Vec3d_mulScalar(&body->velocity, dt);

    /* Apply drag */
    Vec3d_mulScalar(&body->velocity, drag);

    // position = position + velocityForDT;
    newPosition = body->position;
    Vec3d_add(&newPosition, &body->velocity);

    /* Store old position, update position to new position. */
    body->prevPosition = body->position;
    body->position = newPosition;

#if PHYSICS_MOTION_DAMPENING
    PhysBody_dampenSmallMovements(body);
#endif

    /* Reset force. */
    body->prevForce = body->force;
    body->force = (Vec3d){0.0, 0.0, 0.0};
}

void PhysBody_integrateBodies(PhysBody *bodies,
                              int numBodies,
                              float dt,
                              float drag,
                              PhysState *physics)
{
    PhysBody *body;
    int i;
    for (i = 0, body = bodies; i < numBodies; i++, body++)
    {
        if (body->enabled)
        {
            PhysBody_update(body, dt, bodies, numBodies, physics);
        }
    }

    for (i = 0, body = bodies; i < numBodies; i++, body++)
    {
        if (body->enabled /*&& !body->controlled*/)
        {
#if PHYSICS_USE_VERLET_INTEGRATION
            PhysBody_integrateMotionVerlet(body, dt, drag);
#else
            PhysBody_integrateMotionSemiImplicitEuler(body, dt, drag);
#endif
        }
    }

    // do this after so we can fix any world penetration resulting from motion
    // integration
    PhysBehavior_collisionResponse(physics->worldData, bodies, numBodies);
}

void PhysState_step(PhysState *physics,
                    PhysBody *bodies,
                    int numBodies,
                    float dt)
{
    // if dynamic timestep is disabled, use fixed timestep
    if (!physics->dynamicTimestep)
    {
        dt = PHYS_FIXED_TIMESTEP;
    }

    /* Drag is inversely proportional to viscosity. */
    float drag = 1.0 - physics->worldData->viscosity;

    PhysBody_integrateBodies(bodies, numBodies, dt, drag, physics);
}
