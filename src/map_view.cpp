#include "colors.h"
#include "display.h"
#include "geometry.h"
#include "level.h"
#include "player.h"
#include "shapes.h"
#include "utils.h"

#include <algorithm>

namespace map_view {
bool mapView = false;
float zoomLevel = 16.0f;
constexpr float minZoomLevel = 8.0f;
constexpr float maxZoomLevel = 32.0f;
constexpr float stepLevel = 2.0f;

Vector2i GetCenter() { return {display::width / 2, display::height / 2}; }

geometry::Vector2 Remap() {
  const float screenCenterX = static_cast<float>(display::width) / 2.0f;
  const float screenCenterY = static_cast<float>(display::height) / 2.0f;

  const float playerScaledX = player::pos.x * zoomLevel;
  const float playerScaledY = player::pos.y * zoomLevel;

  return geometry::Vector2{screenCenterX - playerScaledX, screenCenterY - playerScaledY};
}

void ZoomMap(const int zoomDirection) {
  zoomLevel += static_cast<float>(zoomDirection) * stepLevel;
  zoomLevel = utils::clamp(zoomLevel, minZoomLevel, maxZoomLevel);
}

void DrawMap() {
  using namespace geometry;
  using namespace level;
  // Vector2 offset = Remap();
  // // draw player arrow in center
  // const Vector2 playerPos = Vector2{player::pos.x * zoomLevel, player::pos.y * zoomLevel} + Remap();

  AddLineInDirectionWithArrow(GetCenter(), 1.0f * zoomLevel, player::rotRad, GREEN_COLOR);
  // AddLineInDirectionWithArrow(static_cast<Vector2i>(playerPos), 12.0f * zoomLevel, player::rotRad, GREEN_COLOR);

  // direction arrow for player
  // if (player::speed != 0) {
  // AddLineInDirectionWithArrow(static_cast<Vector2i>(playerPos), 8.0f * zoomLevel, player::rotRad + player::moveDirRad, BLUE_COLOR);
  // }
  const vector<Wall> &walls = currentLevel.walls;
  for (size_t i = 0; i < walls.size(); i++) {
    const Wall &wall = walls[i];

    const float x1 = wall.a;
    const float y1 = wall.b;
    const float x2 = wall.c;
    const float y2 = wall.d;

    Vector2 ab = {x1 * zoomLevel, y1 * zoomLevel};
    Vector2 cd = {x2 * zoomLevel, y2 * zoomLevel};

    ab = ab + Remap();
    cd = cd + Remap();

    const uint16_t texture = wall.texture;
    uint32_t color = WHITE_COLOR;
    if (texture == 1) {
      color = RED_COLOR;
    } else if (texture == 2) {
      color = GREEN_COLOR;
    } else if (texture == 3) {
      color = BLUE_COLOR;
    }

    AddLine(static_cast<Vector2i>(ab), static_cast<Vector2i>(cd), color);
  }
}

void DrawRay(geometry::Vector2 to) {
  to.x *= zoomLevel;
  to.y *= zoomLevel;
  AddLine(GetCenter(), static_cast<Vector2i>(to + Remap()), YELLOW_COLOR);
}

void ToggleMap() { mapView = !mapView; }

} // namespace map_view