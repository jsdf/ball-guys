#ifndef DRAWOBJ_H
#define DRAWOBJ_H

#include <t3d/t3dmath.h>

#include "jsdf_array.h"

// component system for drawable objects

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
DrawObj *DrawObj_new(
    T3DModel *model);
void DrawObj_draw(int index);
void DrawObj_drawAll();
void DrawObj_cleanup();

T3DMat4FPArray *DrawObj_getTransforms();
DrawObjArray *DrawObj_getAll();
DrawObj *DrawObj_get(int index);

void DrawObj_updateTransform(int index);
void DrawObj_updateTransforms();

#endif // DRAWOBJ_H
