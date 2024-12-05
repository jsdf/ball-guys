#include <libdragon.h>
#include "../../minigame.h"
#include "../../core.h"

#include "./renderer.h"
#include "./game.h"

const MinigameDef minigame_def = {
    .gamename = "Ball Guys",
    .developername = "jsdf",
    .description = "We Ball",
    .instructions = "Last guy standing wins!"};

void minigame_init(void)
{
  Renderer_init();
  Game_init();
}

void minigame_fixedloop(float deltaTime)
{
  Game_fixedUpdate(deltaTime);
}

void minigame_loop(float deltaTime)
{
  Game *game =
      Game_getInstance();
  Renderer_update(
      &game->camPos,
      &game->camTarget);
  // ======== Draw (3D) ======== //
  Renderer_draw();
}

void minigame_cleanup(void)
{
  Game_cleanup();
  Renderer_cleanup();
}
