#ifndef GAME_H
#define GAME_H

#include <t3d/t3dmath.h>

typedef struct Game
{
    T3DVec3 camPos;
    T3DVec3 camTarget;
} Game;

void Game_init();
void Game_fixedUpdate(float deltaTime);
void Game_cleanup();

Game *Game_getInstance();

#endif // GAME_H