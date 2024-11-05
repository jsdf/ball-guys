
#ifndef PHYSICS_H
#define PHYSICS_H

#include "collision.h"
#include "vec3d.h"

#define PHYS_MAX_STEPS 4

#define PHYS_FIXED_TIMESTEP 1.0 / 60.0

typedef struct PhysWorldData
{
    float gravity;
    float viscosity;
    // 'world' collision shapes which do not have physics bodies of their own
    // but can be collided with by physics bodies
    ColliderArray colliders;
} PhysWorldData;

typedef struct PhysState
{
    int dynamicTimestep; // boolean
    PhysWorldData *worldData;
} PhysState;

enum PhysBodyFlags
{
    BoatHullPhysBody = 1 << 0,
    BuoyantPhysBody = 1 << 1,
};

typedef struct PhysBody
{
    int id;
    float mass;
    float massInverse;
    float radius;
    float radiusSquared;
    float restitution;

    int enabled;
    int flags;
    // in verlet, this is used to derive the velocity (pos - prevPos) so changing
    // pos explicitly without changing prevPos will result in acceleration when
    // velocity is derived
    Vec3d prevPosition;
    Vec3d position;
    // in verlet this is derived.
    // after integration, it represents the velocity for dt
    Vec3d velocity;
    // after integration, this represents the velocity for 1 second
    Vec3d nonIntegralVelocity;
    // this is really force (eg. in newtons) until after integration, when it
    // becomes acceleration (verlet)
    Vec3d force;

    // after integration, this represents the acceleration for 1 second
    Vec3d nonIntegralAcceleration;
    // this is the resultant acceleration from the 2x previous timestep
    Vec3d prevForce;
} PhysBody;

void PhysState_init(PhysState *self, PhysWorldData *worldData);

void PhysState_step(PhysState *physics,
                    PhysBody *bodies,
                    int numBodies,
                    float dt // delta time in seconds
);

void PhysBody_init(PhysBody *self,
                   float mass,
                   float radius,
                   Vec3d *position,
                   int id);

void PhysBody_applyForce(PhysBody *body, Vec3d *force);
void PhysBody_translateWithoutForce(PhysBody *body, Vec3d *translation);
void PhysBody_setEnabled(PhysBody *body, int enabled);

#endif /* !PHYSICS_H */
