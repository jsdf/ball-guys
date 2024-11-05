#ifndef PHYSOBJ_H
#define PHYSOBJ_H

// component system for physics objects

#include "physics.h"

#include "jsdf_array.h"

jsdf_array(PhysBody);

void PhysObj_initSystem();
PhysBody *PhysObj_new(
    int id,
    float mass,
    float radius,
    Vec3d position);

void PhysObj_cleanupSystem();

PhysBodyArray *PhysObj_getAll();
PhysBody *PhysObj_get(int index);

#endif // PHYSOBJ_H
