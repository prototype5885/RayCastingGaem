#include "colors.h"
#include "display.h"
#include "geometry.h"
#include "level.h"
#include "player.h"
#include "shapes.h"

#include <algorithm>
#include <format>
#include <iostream>

namespace map_view {
bool mapEnabled = false;
float zoomLevel = 8;
constexpr float minZoomLevel = 1.0f;
constexpr float maxZoomLevel = 16.0f;

geometry::Vector2 Remap() {
  const float xOffset = static_cast<float>(display::width) / 2.0f - player::pos.x;
  const float yOffset = static_cast<float>(display::height) / 2.0f - player::pos.y;
  return geometry::Vector2{xOffset, yOffset};
}

// Vector2i GetCenter() { return Vector2i(display::width / 2, display::height / 2); }

void ZoomMap(const int zoomDirection) {
  zoomLevel += static_cast<float>(zoomDirection);
  zoomLevel = clamp(zoomLevel, minZoomLevel, maxZoomLevel);
  cout << zoomLevel << endl;
}

void DrawMap() {
  using namespace geometry;
  // Vector2 offset = Remap();
  // // draw player arrow in center
  const Vector2 playerPos = {player::pos.x * zoomLevel, player::pos.y * zoomLevel};
  AddLineInDirectionWithArrow(static_cast<Vector2i>(playerPos), 1.0f * zoomLevel, player::rotRad, GREEN_COLOR);
  // AddLineInDirectionWithArrow(static_cast<Vector2i>(playerPos), 12.0f * zoomLevel, player::rotRad, GREEN_COLOR);

  // direction arrow for player
  // if (player::speed != 0) {
  // AddLineInDirectionWithArrow(static_cast<Vector2i>(playerPos), 8.0f * zoomLevel, player::rotRad + player::moveDirRad, BLUE_COLOR);
  // }

  for (auto [x1, y1, x2, y2] : level::walls) {
    Vector2 ab = {x1 * zoomLevel, y1 * zoomLevel};
    Vector2 cd = {x2 * zoomLevel, y2 * zoomLevel};

    AddLine(static_cast<Vector2i>(ab), static_cast<Vector2i>(cd), WHITE_COLOR);
  }
}

void DrawRays(const float angle, const float distance) {
  using namespace geometry;
  const Vector2 from = {player::pos.x * zoomLevel, player::pos.y * zoomLevel};
  AddLineInDirection(static_cast<Vector2i>(from), distance * zoomLevel * 8, angle, RED_COLOR);
}

void ToggleMap() { mapEnabled = !mapEnabled; }

} // namespace map_view