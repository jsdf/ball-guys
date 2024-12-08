
#include <stdlib.h>
#include "jsdf_array.h"

#include "physobj.h"

#define MAX_PHYS_OBJS 10

jsdf_array_funcs(PhysBody, malloc, free);

PhysBodyArray physBodies;

void PhysObj_initSystem()
{
    PhysBodyArray_reserve(&physBodies, MAX_PHYS_OBJS);
}

PhysBody *PhysObj_new(
    int id,
    float mass,
    float radius,
    Vec3d position)
{
    PhysBody body;
    PhysBody_init(&body, mass, radius, &position, id);
    PhysBodyArray_push(&physBodies, body);
    return PhysObj_get(physBodies.length - 1);
}
inline PhysBodyArray *PhysObj_getAll()
{
    return &physBodies;
}

inline PhysBody *PhysObj_get(int index)
{
    return PhysBodyArray_at(&physBodies, index);
}

PhysBody *PhysObj_getByID(int id)
{
    PhysBody *body;
    size_t i;
    jsdf_array_foreach_ptr(&physBodies, body, i)
    {
        if (body->id == id)
        {
            return body;
        }
    }
    return NULL;
}

void PhysObj_cleanupSystem()
{
    PhysBodyArray_free(&physBodies);
}
