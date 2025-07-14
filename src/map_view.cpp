#include "display.h"
#include "extra_math.h"
#include "player.h"
#include "shapes.h"

int zoomValue = 8;

namespace map_view {

// map
bool mapEnabled = false;

void DrawMap() {
  // constexpr int mapWidth = 16;
  // constexpr int mapHeight = 16;

  for (int s = 0; s < display::size; s++) {
    const int x = s % display::width;
    const int y = s / display::width;

    // AddPixelToBuffer(x + x * 8, y + y * 8, 0x28);
  }

  auto playerPosOnMap = Vector2i(display::width / 2, display::height / 2);
  // playerPosOnMap.x = static_cast<int>(player::pos.x + player::pos.x * 8);
  // playerPosOnMap.y = static_cast<int>(player::pos.y + player::pos.y * 8);

  // // draw player arrow in center
  AddLineInDirectionWithArrow(playerPosOnMap, 12.0f * static_cast<float>(zoomValue), player::rotRad, 0x28);

  // direction arrow for player
  if (player::speed != 0) {
    AddLineInDirectionWithArrow(playerPosOnMap, 8.0f * static_cast<float>(zoomValue), player::rotRad + player::moveDirRad, 0x2f);
  }
}

void ToggleMap(bool *mapEnabled) { *mapEnabled = !*mapEnabled; }

} // namespace map_view