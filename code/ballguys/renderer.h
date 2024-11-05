#ifndef BALLGUYS_RENDERER_H
#define BALLGUYS_RENDERER_H

#include <t3d/t3dmath.h>
// prototypes
void Renderer_init();
void Renderer_update(
    T3DVec3 *camPos,
    T3DVec3 *camTarget);
void Renderer_draw();
void Renderer_cleanup();

#endif // BALLGUYS_RENDERER_H