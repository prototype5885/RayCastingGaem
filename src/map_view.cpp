#include "colors.h"
#include "display.h"
#include "geometry.h"
#include "level.h"
#include "player.h"
#include "shapes.h"

#include <algorithm>
#include <format>
#include <iostream>
#include <ostream>

namespace map_view {
uint8_t mapViewMode = 0;
float zoomLevel = 16.0f;
constexpr float minZoomLevel = 8.0f;
constexpr float maxZoomLevel = 32.0f;
constexpr float stepLevel = 2.0f;

Vector2i GetCenter() { return Vector2i(display::width / 2, display::height / 2); }

geometry::Vector2 Remap() {
  const float screenCenterX = static_cast<float>(display::width) / 2.0f;
  const float screenCenterY = static_cast<float>(display::height) / 2.0f;

  const float playerScaledX = player::pos.x * zoomLevel;
  const float playerScaledY = player::pos.y * zoomLevel;

  return geometry::Vector2{screenCenterX - playerScaledX, screenCenterY - playerScaledY};
}

void ZoomMap(const int zoomDirection) {
  zoomLevel += static_cast<float>(zoomDirection) * stepLevel;
  zoomLevel = clamp(zoomLevel, minZoomLevel, maxZoomLevel);
}

void DrawMap() {
  using namespace geometry;
  // Vector2 offset = Remap();
  // // draw player arrow in center
  // const Vector2 playerPos = Vector2{player::pos.x * zoomLevel, player::pos.y * zoomLevel} + Remap();

  AddLineInDirectionWithArrow(static_cast<Vector2i>(GetCenter()), 1.0f * zoomLevel, player::rotRad, GREEN_COLOR);
  // AddLineInDirectionWithArrow(static_cast<Vector2i>(playerPos), 12.0f * zoomLevel, player::rotRad, GREEN_COLOR);

  // direction arrow for player
  // if (player::speed != 0) {
  // AddLineInDirectionWithArrow(static_cast<Vector2i>(playerPos), 8.0f * zoomLevel, player::rotRad + player::moveDirRad, BLUE_COLOR);
  // }

  for (auto [x1, y1, x2, y2] : level::walls) {
    Vector2 ab = {x1 * zoomLevel, y1 * zoomLevel};
    Vector2 cd = {x2 * zoomLevel, y2 * zoomLevel};

    ab = ab + Remap();
    cd = cd + Remap();

    AddLine(static_cast<Vector2i>(ab), static_cast<Vector2i>(cd), WHITE_COLOR);
  }
}

void DrawRays(const float angle, const float distance) {
  using namespace geometry;
  const Vector2 from = {player::pos.x * zoomLevel, player::pos.y * zoomLevel};
  AddLineInDirection(static_cast<Vector2i>(from), distance * zoomLevel * 8, angle, RED_COLOR);
}

void ToggleMapMode() { mapViewMode = mapViewMode >= 2 ? 0 : mapViewMode + 1; }

} // namespace map_view