#ifndef DRAWOBJ_H
#define DRAWOBJ_H

#include <libdragon.h>

#include <t3d/t3d.h>
#include <t3d/t3dmath.h>
#include <t3d/t3dmodel.h>

#include "./jsdf_array.h"

typedef struct DrawObj
{
    int id;
    T3DVec3 position;
    T3DVec3 rotation;
    rspq_block_t *drawBlock;
} DrawObj;

jsdf_array(T3DMat4FP);
jsdf_array(DrawObj);

void DrawObj_init();
void DrawObj_new(
    int index,
    T3DModel *model);
void DrawObj_draw(int index);
void DrawObj_drawAll();
void DrawObj_cleanup();

T3DMat4FPArray *DrawObj_getTransforms();
DrawObjArray *DrawObj_get();

void DrawObj_updateTransform(int index);
void DrawObj_updateTransforms();

#endif // DRAWOBJ_H
