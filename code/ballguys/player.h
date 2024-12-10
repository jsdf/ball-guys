#ifndef PLAYER_H
#define PLAYER_H

#include <libdragon.h>
#include "vec3d.h"
#include "jsdf_array.h"

#include "../../minigame.h"
#include "../../core.h"

typedef struct Player
{
    PlyNum plynum;
    int objID;
    bool charging;
    int chargePower;
    Vec3d input; // using only x and z
    Vec3d localOffset;
} Player;

jsdf_array(Player);

void Player_initSystem();

Player *Player_getPlayer(PlyNum plynum);
PlayerArray *Player_getAll();

void Player_init(Player *player, PlyNum plynum, uint32_t objID);
void Player_updateInput(Player *player, float deltaTime, joypad_port_t port);

#endif /* !PLAYER_H */