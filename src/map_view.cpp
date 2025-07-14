#include "display.h"
#include "player.h"
#include "shapes.h"
#include "structs.h"

void DrawMap() {
  constexpr int mapWidth = 16;
  constexpr int mapHeight = 16;

  for (int s = 0; s < mapWidth * mapHeight; s++) {
    const int x = s % mapWidth;
    const int y = s / mapWidth;

    AddPixelToBuffer(x + x * 8, y + y * 8, 0x28);
  }

  Vector2i playerPosOnMap;
  playerPosOnMap.x = static_cast<int>(player::pos.x + player::pos.x * 8);
  playerPosOnMap.y = static_cast<int>(player::pos.y + player::pos.y * 8);

  // // draw player arrow in center
  AddLineInDirectionWithArrow(playerPosOnMap, 12.0f, player::rotRad, 0x28);

  // direction arrow for player
  if (player::speed != 0) {
    AddLineInDirectionWithArrow(playerPosOnMap, 8.0f, player::rotRad + player::moveDirRad, 0x2f);
  }
}

void ToggleMap(bool *mapEnabled) { *mapEnabled = !*mapEnabled; }