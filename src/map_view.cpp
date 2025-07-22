#include "colors.h"
#include "display.h"
#include "geometry.h"
#include "level.h"
#include "player.h"
#include "shapes.h"
#include "utils.h"

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
  const vector<Sector> &sectors = currentLevel.sectors;
  for (size_t s = 0; s < sectors.size(); s++) {
    const vector<Wall> &walls = sectors.at(s).walls;
    for (size_t i = 0; i < walls.size(); i++) {
      const Wall &wall = walls[i];

      if (!wall.collision)
        continue;

      constexpr uint32_t color = WHITE_COLOR;

      const Vector2 ab = Vector2{wall.from.x * zoomLevel, wall.from.y * zoomLevel} + Remap();
      const Vector2 cd = Vector2{wall.to.x * zoomLevel, wall.to.y * zoomLevel} + Remap();

      AddLine(static_cast<Vector2i>(ab), static_cast<Vector2i>(cd), color);
    }
  }
}

void DrawRay(geometry::Vector2 to) {
  to.x *= zoomLevel;
  to.y *= zoomLevel;
  AddLine(GetCenter(), static_cast<Vector2i>(to + Remap()), YELLOW_COLOR);
}

void ToggleMap() { mapView = !mapView; }

} // namespace map_view