#include "Colors.h"
#include "Display.h"
#include "Player.h"
#include "Shapes.h"
#include "Structs.h"
#include <cstdint>

void DrawMap(DisplayData *dd, int8_t *map, Player *player) {
  const int mapWidth = 16;
  const int mapHeight = 16;

  for (int s = 0; s < mapWidth * mapHeight; s++) {
    const int x = s % mapWidth;
    const int y = s / mapWidth;

    AddPixelToBuffer(dd, x + x * 8, y + y * 8, RED_COLOR);
  }

  Vector2i playerPosOnMap;
  playerPosOnMap.x = player->pos.x + player->pos.x * 8;
  playerPosOnMap.y = player->pos.y + player->pos.y * 8;

  // // draw player arrow in center
  AddLineInDirectionWithArrow(dd, playerPosOnMap, 12.0f, player->rotRad, RED_COLOR);

  // direction arrow for player
  if (player->speed != 0) {
    AddLineInDirectionWithArrow(dd, playerPosOnMap, 8.0f, player->rotRad + player->moveDirRad, GREEN_COLOR);
  }
}

void ToggleMap(bool *mapEnabled) { *mapEnabled = !(*mapEnabled); }