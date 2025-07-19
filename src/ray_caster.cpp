#include "ray_caster.h"
#include "colors.h"
#include "display.h"
#include "geometry.h"
#include "level.h"
#include "player.h"
#include "texture.h"
#include "utils.h"

#include <algorithm>
#include <cfloat>
#include <cmath>
#include <cstdint>
#include <iostream>
#include <vector>

using namespace std;
using namespace geometry;

#define MAX_RAY_DISTANCE 100

typedef struct {
  float minDistance;
  Vector2 hitPoint;
  float part;
  string texture;
  float wallLength;
} RayHitPoint;

typedef struct {
  Vector2 point;
  float part;
} Intersection;

struct WallSlice {
  int wallX;
  float distance, part;
  const level::Wall *wall;

  bool operator<(const WallSlice &other) const { return distance > other.distance; }
};

bool CompareByDistance(const WallSlice &a, const WallSlice &b) { return a.distance < b.distance; }

Intersection LineIntersection(const float x1, const float y1, const float x2, const float y2, const float x3, const float y3, const float x4,
                              const float y4) {
  const float den = (x1 - x2) * (y3 - y4) - (y1 - y2) * (x3 - x4);
  if (den == 0)
    return {{FLT_MAX, FLT_MAX}, 0.0f};
  const float t = ((x1 - x3) * (y3 - y4) - (y1 - y3) * (x3 - x4)) / den;
  const float u = -((x1 - x2) * (y1 - y3) - (y1 - y2) * (x1 - x3)) / den;
  if (t >= 0 && t <= 1 && u >= 0 && u <= 1) {
    const float px = x1 + t * (x2 - x1);
    const float py = y1 + t * (y2 - y1);
    return {{px, py}, u};
  }
  return {{FLT_MAX, FLT_MAX}, 0.0f};
}

float PointToLineDistance(const float px, const float py, const float x1, const float y1, const float x2, const float y2) {
  const float dx = x2 - x1;
  const float dy = y2 - y1;
  const float lenSquared = powf(dx, 2) + powf(dy, 2);
  if (lenSquared == 0) {
    return EuclideanDistance({px, py}, {x1, y1});
  }
  float t = ((px - x1) * dx + (py - y1) * dy) / lenSquared;
  t = max(0.0f, min(1.0f, t));
  const float projX = x1 + t * dx;
  const float projY = y1 + t * dy;
  return EuclideanDistance({px, py}, {projX, projY});
}

inline void DrawWallSlice(const WallSlice &wallSlice) {
  // calculate wall position and dimension
  const float wallHeight = static_cast<float>(display::height) / wallSlice.distance;
  const float wallMiddle = static_cast<float>(display::height) / 2.0f - player::z / wallSlice.distance + player::rotVerticalRad;

  const float wallStart = wallMiddle - wallHeight / 2.0f;
  const float wallEnd = wallMiddle + wallHeight / 2.0f;

  // fog like shading
  constexpr float minPercentage = 1.0f;
  constexpr float maxPercentage = 16.0f;
  float percentage = 1.0f - (wallSlice.distance - minPercentage) / (maxPercentage - minPercentage);
  if (wallSlice.distance < minPercentage) {
    percentage = 1.0f;
  } else if (wallSlice.distance > maxPercentage) {
    percentage = 0.0f;
  }
  if (percentage < 0.25f)
    percentage = 0.25f;

  // calculate which pixel column is needed for this ray
  const texture::Texture *texture = &texture::textureList.at(wallSlice.wall->texture);
  int textureX = static_cast<int>(static_cast<float>(texture->width) * wallSlice.part * wallSlice.wall->wallLength);
  textureX = textureX % texture->width;
  textureX = utils::clamp(textureX, 0, texture->height - 1);

  // clamp the wall so it stays between display only
  const int realWallStart = utils::clamp(static_cast<int>(wallStart), 0, display::height);
  const int realWallEnd = utils::clamp(static_cast<int>(wallEnd), 0, display::height);

  for (int wallY = realWallStart; wallY < realWallEnd; wallY++) {
    // calculate which pixel needs to be grabbed from the pixel column
    const float normalizedTextureY = (static_cast<float>(wallY) - wallStart) / wallHeight;
    int textureY = static_cast<int>(normalizedTextureY * static_cast<float>(texture->height));

    // to prevent it from accessing +1 above limit, sometimes it happened
    textureY = utils::clamp(textureY, 0, texture->height - 1);

    const int pos = textureY * texture->width + textureX;
    uint32_t color = texture->colors.at(pos);
    color = color::MultiplyRGB(color, percentage);

    display::AddPixelToBuffer(wallSlice.wallX, wallY, color);
  }
}

void ray_caster::CastRays() {
  using namespace level;

  const float fov = static_cast<float>(display::width) / static_cast<float>(display::height);
  const float startAngle = player::rotRad - fov / 2.0f;
  const float angleStep = fov / static_cast<float>(display::width);

  for (int ray = 0; ray < display::width; ray++) {
    const float rayAngle = startAngle + static_cast<float>(ray) * angleStep;

    const float rayX = cosf(rayAngle);
    const float rayY = sinf(rayAngle);

    vector<WallSlice> intersectedWalls;

    const vector<Wall> &walls = currentLevel.walls;
    for (int i = 0; i < static_cast<int>(currentLevel.walls.size()); i++) {
      const Wall &wall = walls[i];

      const Intersection intersection = LineIntersection(player::pos.x, player::pos.y, player::pos.x + rayX * MAX_RAY_DISTANCE,
                                                         player::pos.y + rayY * MAX_RAY_DISTANCE, wall.a, wall.b, wall.c, wall.d);

      if (intersection.point.x != FLT_MAX) {
        float distance = EuclideanDistance(intersection.point, player::pos);
        distance = distance * cosf(rayAngle - player::rotRad); // fisheye correction

        intersectedWalls.push_back({ray, distance, intersection.part, &wall});

        // if (map_view::mapView) {
        // map_view::DrawRay(intersection.point);
        // } else {
        // DrawWallSlice(ray, distance, intersection.part, currentLevel.walls[i].texture, wall.wallLength);
        // }
      }
    }

    std::sort(intersectedWalls.begin(), intersectedWalls.end());

    for (size_t i = 0; i < intersectedWalls.size(); i++) {
      const WallSlice *wallSlice = &intersectedWalls[i];
      DrawWallSlice(*wallSlice);
    }
  }
}