
#include <libdragon.h>
#include <t3d/t3d.h>
#include <t3d/t3dmath.h>
#include <t3d/t3dmodel.h>
#include <t3d/t3ddebug.h>
#include "./drawobj.h"
#include "./renderer.h"

surface_t *depthBuffer;
T3DViewport viewport;

T3DVec3 camPos;
T3DVec3 camTarget;
T3DVec3 lightDirVec;

rspq_syncpoint_t syncPoint;

void Renderer_init()
{

    display_init(RESOLUTION_640x480, DEPTH_16_BPP, 3, GAMMA_NONE, FILTERS_RESAMPLE);
    depthBuffer = display_get_zbuf();

    t3d_init((T3DInitParams){});

    viewport = t3d_viewport_create();

    camPos = (T3DVec3){{0, 125.0f, 100.0f}};
    camTarget = (T3DVec3){{0, 0, 40}};

    lightDirVec = (T3DVec3){{1.0f, 1.0f, 1.0f}};
    t3d_vec3_norm(&lightDirVec);

    syncPoint = 0;
}

void Renderer_update()
{
    // update before drawing
    t3d_viewport_set_projection(&viewport, T3D_DEG_TO_RAD(90.0f), 20.0f, 160.0f);
    t3d_viewport_look_at(&viewport, &camPos, &camTarget, &(T3DVec3){{0, 1, 0}});

    // update transforms used by renderer based on drawobj structs
    DrawObj_updateTransforms();
}
void Renderer_draw()
{

    uint8_t colorAmbient[4] = {0xAA, 0xAA, 0xAA, 0xFF};
    uint8_t colorDir[4] = {0xFF, 0xAA, 0xAA, 0xFF};

    rdpq_attach(display_get(), depthBuffer);
    t3d_frame_start();
    t3d_viewport_attach(&viewport);

    t3d_screen_clear_color(RGBA32(52, 232, 235, 0xFF));
    t3d_screen_clear_depth();

    t3d_light_set_ambient(colorAmbient);
    t3d_light_set_directional(0, colorDir, &lightDirVec);
    t3d_light_set_count(1);

    DrawObj_drawAll();

    syncPoint = rspq_syncpoint_new();

    rdpq_sync_tile();
    rdpq_sync_pipe(); // Hardware crashes otherwise

    rdpq_detach_show();
}

void Renderer_cleanup()
{
    t3d_destroy();

    display_close();
}