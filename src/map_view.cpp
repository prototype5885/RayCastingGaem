#include "display.h"
#include "extra_math.h"
#include "player.h"
#include "shapes.h"

#include <iostream>

using namespace std;

namespace map_view {

bool mapEnabled = false;
int zoomValue = 8;

Vector2i GetCenter() { return Vector2i(display::width / 2, display::height / 2); }

void ZoomMap(int zoomDirection) {
  zoomValue += zoomDirection;
  zoomValue = clampi(zoomValue, 1, 8);
  cout << zoomValue << endl;
}

void DrawMap() {
  // // draw player arrow in center
  AddLineInDirectionWithArrow(GetCenter(), 12.0f * static_cast<float>(zoomValue), player::rotRad, 0x28);

  // direction arrow for player
  if (player::speed != 0) {
    AddLineInDirectionWithArrow(GetCenter(), 8.0f * static_cast<float>(zoomValue), player::rotRad + player::moveDirRad, 0x2f);
  }
}

void DrawRays(float angle, float distance) { AddLineInDirection(GetCenter(), distance * static_cast<float>(zoomValue) * 8, angle, 0x28); }

void ToggleMap() { mapEnabled = !mapEnabled; }

} // namespace map_view