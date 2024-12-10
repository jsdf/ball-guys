#include <t3d/t3d.h>
#include <t3d/t3dmodel.h>
#include "drawobj.h"
#include "game.h"
#include "physics.h"
#include "physobj.h"
#include "player.h"
#include "vec3d.h"
#include "../../minigame.h"
#include "../../core.h"

T3DModel *modelBox;
T3DModel *modelBall;
T3DModel *modelDisc;

PhysWorldData worldData = {
    .gravity = -9.8f,
    .viscosity = 0.05f,
};
PhysState physics;
Game game;

Player players[MAXPLAYERS];

PlyNum winner;

#define MOVE_SPEED 1000.0f

#define T3DVec3ApplyVec3d(t3dVec, vec, op) \
    t3dVec.v[0] op vec.x;                  \
    t3dVec.v[1] op vec.y;                  \
    t3dVec.v[2] op vec.z;

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

    for (size_t i = 0; i < MAXPLAYERS; i++)
    {
        DrawObj *box = DrawObj_new(modelBall);
        // first argument is object id
        // this is used to relate the physics object to the draw object
        PhysObj_new(box->id, 100.0f, 60.0f, (Vec3d){0.0f, 0.0f, 0.0f});
        Player_init(&players[i], i, box->id);
    }
    DrawObj *disc = DrawObj_new(modelDisc);

    // iterate players
    for (size_t i = 0; i < MAXPLAYERS; i++)
    {
        // get player physics object
        PhysBody *playerPhysObj = PhysObj_get(players[i].objID);
        switch (i)
        {
        case 0:
            playerPhysObj->position = (Vec3d){200.0f, 10.0f, 10.0f};
            break;
        case 1:
            playerPhysObj->position = (Vec3d){0.0f, 10.0f, 10.0f};
            break;
        case 2:
            playerPhysObj->position = (Vec3d){0.0f, 10.0f, 200.0f};
            break;
        case 3:
            playerPhysObj->position = (Vec3d){200.0f, 10.0f, 200.0f};
            break;
        }
    }
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
        T3DVec3ApplyVec3d(drawObj->position, physBody->position, =);
    }
}
void Game_updatePlayer(Player *player, float deltaTime, joypad_port_t port, bool is_human)
{
    if (is_human)
    {
        Player_updateInput(
            player,
            deltaTime,
            port);
    }

    // get physics object
    PhysBody *physBody = PhysObj_getByID(player->objID);
    // move object
    Vec3d translation = {player->input.x * MOVE_SPEED * deltaTime, 0.0f, player->input.z * MOVE_SPEED * deltaTime};
    PhysBody_translateWithoutForce(
        physBody,
        &translation);
}

static int frame = 0;
void Game_fixedUpdate(
    float deltaTime // in seconds
)
{
    frame++;
    debugf("----\nframe %d\n", frame);

    uint32_t playercount = core_get_playercount();
    for (size_t i = 0; i < MAXPLAYERS; i++)
    {
        Game_updatePlayer(&players[i], deltaTime, core_get_playercontroller(i), i < playercount);
    }

    Game_updatePhysObjs(deltaTime);

    // iterate player draw objects
    for (size_t i = 0; i < MAXPLAYERS; i++)
    {
        DrawObj *drawObj = DrawObj_get(players[i].objID);
        // update the draw object's transform
        DrawObj_updateTransform(players[i].objID);

        {
            // rotate
            drawObj->rotation.v[i % 3] += 0.1f;

            // add local offset
            T3DVec3ApplyVec3d(drawObj->position, players[i].localOffset, +=);
        }
    }

    size_t numDrawObjs = DrawObj_getAll()->length;
    DrawObj *drawObj;
    for (size_t i = 0; i < numDrawObjs; i++)
    {
        drawObj = DrawObj_get(i);
        debugf("drawObj %d pos %f %f %f\n", drawObj->id, drawObj->position.v[0], drawObj->position.v[1], drawObj->position.v[2]);
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