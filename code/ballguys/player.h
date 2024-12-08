#ifndef PLAYER_H
#define PLAYER_H

#include "vec3d.h"

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

void Player_init(Player *player, PlyNum plynum, uint32_t objID);
void Player_updateInput(Player *player, float deltaTime, joypad_port_t port);

#endif /* !PLAYER_H */