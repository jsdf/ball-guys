#include <t3d/t3d.h>
#include <t3d/t3dmodel.h>
#include "drawobj.h"
#include "game.h"
#include "physics.h"
#include "physobj.h"
#include "vec3d.h"

T3DModel *modelBox;
T3DModel *modelBall;
T3DModel *modelDisc;

PhysWorldData worldData = {
    .gravity = -9.8f,
    .viscosity = 0.05f,
};
PhysState physics;
Game game;

#define MOVE_SPEED 0.1f

inline void Game_copyT3DVec3FromVec3d(T3DVec3 *t3dVec, Vec3d *vec)
{
    t3dVec->v[0] = vec->x;
    t3dVec->v[1] = vec->y;
    t3dVec->v[2] = vec->z;
}

void Game_init()
{
    game.camPos = (T3DVec3){{0, 600.0f, 400.0f}};
    game.camTarget = (T3DVec3){{0, 0, 0}};

    DrawObj_init();
    ColliderArray_reserve(&worldData.colliders, 1);
    // TODO: world colliders need a position
    ColliderArray_push(
        &worldData.colliders,
        (Collider){
            .type = COLLIDER_PLANE,
            .position = (Vec3d){0.0f, 0.0f, 0.0f},
            .shape = {
                .plane = {
                    // normal is up
                    .normal = {0.0f, 1.0f, 0.0f},
                }}});
    PhysState_init(&physics, &worldData);
    PhysObj_initSystem();

    modelBox = t3d_model_load("rom:/ballguys/box.t3dm");
    modelBall = t3d_model_load("rom:/ballguys/ball.t3dm");
    modelDisc = t3d_model_load("rom:/ballguys/disc.t3dm");

    DrawObj *box1 = DrawObj_new(modelBox);
    DrawObj *box2 = DrawObj_new(modelBox);
    DrawObj *disc = DrawObj_new(modelDisc);
    // first argument is object id
    // this is used to relate the physics object to the draw object
    PhysObj_new(box1->id, 1.0f, 2.0f, (Vec3d){200.0f, 10.0f, 10.0f});
    PhysObj_new(box2->id, 1.0f, 2.0f, (Vec3d){0.0f, 10.0f, 10.0f});
}

void Game_updatePhysObjs(
    float dt)
{
    // first integrate motion for all objects
    // then copy the new positions to the draw objects
    PhysBodyArray *physBodies = PhysObj_getAll();

    PhysState_step(&physics, &physBodies->data[0], physBodies->length, dt);

    DrawObj *drawObj;
    PhysBody *physBody;
    size_t i;

    for (i = 0; i < physBodies->length; i++)
    {
        physBody = PhysObj_get(i);
        // get the draw object associated with this physics object
        drawObj = DrawObj_get(physBody->id);
        // copy the new position to the draw object
        Game_copyT3DVec3FromVec3d(&drawObj->position, &physBody->position);
    }
}

void Game_fixedUpdate(
    float deltaTime // in seconds
)
{
    Game_updatePhysObjs(deltaTime);

    size_t numDrawObjs = DrawObj_getAll()->length;
    DrawObj *drawObj;
    for (size_t i = 0; i < numDrawObjs; i++)
    {
        drawObj = DrawObj_get(i);
        debugf("drawObj %d pos %f %f %f\n", drawObj->id, drawObj->position.v[0], drawObj->position.v[1], drawObj->position.v[2]);

        // rotate
        if (drawObj->id == 0 || drawObj->id == 1)
        {
            //
            drawObj->rotation.v[0] += 0.1f;
        }
    }
}

void Game_cleanup()
{

    DrawObj_cleanup();
    PhysObj_cleanupSystem();

    t3d_model_free(modelBox);
}

Game *Game_getInstance()
{
    return &game;
}