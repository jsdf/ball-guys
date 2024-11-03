
#include "./drawobj.h"
#include "./game.h"

T3DModel *modelBox;

#define MOVE_SPEED 0.1f

void Game_init()
{
    DrawObj_init();

    modelBox = t3d_model_load("rom:/ballguys/box.t3dm");

    DrawObj_new(0, modelBox);
    DrawObj_new(1, modelBox);
    DrawObj_new(2, modelBox);
}

void Game_fixedUpdate(float deltaTime)
{

    DrawObjArray *drawObjs = DrawObj_get();
    DrawObj *drawObj;
    size_t i;
    for (i = 0; i < drawObjs->length; i++)
    {
        drawObj = DrawObjArray_at(drawObjs, i);
        // update object positions
        T3DVec3 *position = &drawObj->position;

        // rotate
        T3DVec3 *rotation = &drawObj->rotation;
        rotation->v[1] += 0.1f;
    }
}

void Game_cleanup()
{

    DrawObj_cleanup();

    t3d_model_free(modelBox);
}