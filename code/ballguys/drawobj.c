
#include <libdragon.h>
#include <t3d/t3d.h>
#include <t3d/t3dmath.h>
#include <t3d/t3dmodel.h>
#include "./drawobj.h"

#define MAX_DRAW_OBJS 10

T3DVec3 defaultScale = {{0.2f, 0.2f, 0.2f}};

jsdf_array_funcs(T3DMat4FP, malloc_uncached, free_uncached);
jsdf_array_funcs(DrawObj, malloc, free);

T3DMat4FPArray objTransforms;
DrawObjArray drawObjs;

void T3DVec3_set(T3DVec3 *v, float x, float y, float z)
{
    v->v[0] = x;
    v->v[1] = y;
    v->v[2] = z;
}

// initialize the draw object system
void DrawObj_init()
{
    T3DMat4FPArray_reserve(&objTransforms, MAX_DRAW_OBJS);
    DrawObjArray_reserve(&drawObjs, MAX_DRAW_OBJS);
}

// set up an object with everything we can statically define
// so we can just draw it later
void DrawObj_new(
    int index,
    T3DModel *model)
{
    int id = objTransforms.length;
    T3DMat4FP *transform = T3DMat4FPArray_push_uninit(&objTransforms);
    rspq_block_begin();

    t3d_matrix_push(transform);
    rdpq_set_prim_color(RGBA32(255, 255, 255, 255));
    t3d_model_draw(model);
    t3d_matrix_pop(1);
    rspq_block_t *drawModelBlock = rspq_block_end();

    DrawObj *drawObj = DrawObjArray_push_uninit(&drawObjs);
    drawObj->id = id;
    drawObj->position = (T3DVec3){{0, 0, 0}};
    drawObj->rotation = (T3DVec3){{0, 0, 0}};

    drawObj->drawBlock = drawModelBlock;
    DrawObj_updateTransform(index);
}

// draw a single object by index
void DrawObj_draw(int index)
{
    DrawObj *drawObj = DrawObjArray_at(&drawObjs, index);
    rspq_block_run(drawObj->drawBlock);
}

void DrawObj_drawAll()
{
    for (size_t i = 0; i < drawObjs.length; i++)
    {
        DrawObj_draw(i);
    }
}

// clean up all the draw objects
void DrawObj_cleanup()
{
    for (size_t i = 0; i < drawObjs.length; i++)
    {
        DrawObj *drawObj = DrawObjArray_at(&drawObjs, i);
        // free the draw block for this object
        rspq_block_free(drawObj->drawBlock);
    }
    DrawObjArray_free(&drawObjs);

    // free the transforms used by the draw objects
    T3DMat4FPArray_free(&objTransforms);
}

T3DMat4FPArray *DrawObj_getTransforms()
{
    return &objTransforms;
}

DrawObjArray *DrawObj_get()
{
    return &drawObjs;
}

void DrawObj_updateTransform(
    int index)
{
    DrawObj *drawObj = DrawObjArray_at(&drawObjs, index);
    T3DMat4FP *transform = T3DMat4FPArray_at(&objTransforms, index);
    t3d_mat4fp_from_srt_euler(
        transform,
        defaultScale.v,      // scale
        drawObj->rotation.v, // rotation
        drawObj->position.v  // translation
    );
}

void DrawObj_updateTransforms()
{
    for (size_t i = 0; i < drawObjs.length; i++)
    {
        DrawObj_updateTransform(i);
    }
}