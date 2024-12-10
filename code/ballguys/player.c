
#include "player.h"
#include <stdint.h>
#include <joypad.h>

#include "macros.h"

#define DEAD_ZONE 0.1f

void Player_init(Player *player, PlyNum plynum, uint32_t objID)
{
    player->plynum = plynum;
    player->objID = objID;
    player->charging = false;
    player->chargePower = 0;
    player->input = (Vec3d){0.0f, 0.0f, 0.0f};
    player->localOffset = (Vec3d){0.0f, 0.0f, 0.0f};
}

void Player_updateInput(Player *player, float deltaTime, joypad_port_t port)
{

    joypad_buttons_t btn = joypad_get_buttons(port);
    joypad_buttons_t btn_released = joypad_get_buttons_released(port);
    joypad_inputs_t inputs =
        joypad_get_inputs(port);

    if (btn.start)
        minigame_end();

    // Player Attack
    if ((btn.a || btn.b) && !player->charging)
    {
        player->charging = true;
        player->chargePower = 0;
    }
    if (player->charging)
    {
        player->chargePower += 1;
        if (player->chargePower > 100)
        {
            player->chargePower = 100;
        }
    }

    if ((btn_released.a || btn_released.b))
    {
        // launch
    }

    // Player Movement
    player->input = (Vec3d){0.0f, 0.0f, 0.0f};
    // multiply by 2 to account for limited range of stick input
    player->input.x =
        CLAMP(
            inputs.stick_x / 127.0f * 2.0f, -1.0f, 1.0f);
    player->input.z =
        CLAMP(
            -inputs.stick_y / 127.0f * 2.0f, -1.0f, 1.0f);

    Vec3d_normalise(&player->input);

    float magnitudeSquared = Vec3d_magSq(&player->input);
    if (magnitudeSquared < (DEAD_ZONE * DEAD_ZONE))
    {
        player->input = (Vec3d){0.0f, 0.0f, 0.0f};
    }
}