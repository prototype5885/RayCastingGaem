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
#include <thread>
#include <vector>

using namespace std;
using namespace geometry;

namespace ray_caster {
bool multiThreaded = true;
}

#define MAX_RAY_DISTANCE 100

typedef struct {
  float minDistance;
  Vector2 hitPoint;
  float where;
  string texture;
  float wallLength;
} RayHitPoint;

struct WallSlice {
  int wallX;
  float distance, where;
  const level::Wall *wall;

  bool operator<(const WallSlice &other) const { return distance > other.distance; }
};

// bool CompareByDistance(const WallSlice &a, const WallSlice &b) { return a.distance < b.distance; }

// float PointToLineDistance(const float px, const float py, const float x1, const float y1, const float x2, const float y2) {
//   const float dx = x2 - x1;
//   const float dy = y2 - y1;
//   const float lenSquared = powf(dx, 2) + powf(dy, 2);
//   if (lenSquared == 0) {
//     return EuclideanDistance({px, py}, {x1, y1});
//   }
//   float t = ((px - x1) * dx + (py - y1) * dy) / lenSquared;
//   t = max(0.0f, min(1.0f, t));
//   const float projX = x1 + t * dx;
//   const float projY = y1 + t * dy;
//   return EuclideanDistance({px, py}, {projX, projY});
// }

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
  int textureX = static_cast<int>(static_cast<float>(texture->width) * wallSlice.where * wallSlice.wall->wallLength);
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
    color = color::DarkenColor(color, percentage);

    display::AddPixelToBuffer(wallSlice.wallX, wallY, color);
  }
}

void CastRay(const int ray, const float startAngle, const float angleStep) {
  using namespace level;
  const float rayAngle = startAngle + static_cast<float>(ray) * angleStep;
  const Vector2 dirVector = GetForwardVector(rayAngle);

  vector<WallSlice> intersectedWalls;
  const vector<Wall> &walls = currentLevel.walls;
  for (int i = 0; i < static_cast<int>(currentLevel.walls.size()); i++) {
    const Wall &wall = walls[i];

    const Intersection intersection = LineIntersection(player::pos.x, player::pos.y, player::pos.x + dirVector.x * MAX_RAY_DISTANCE,
                                                       player::pos.y + dirVector.y * MAX_RAY_DISTANCE, wall.a, wall.b, wall.c, wall.d);
    if (intersection.point.x != FLT_MAX) {
      float distance = EuclideanDistance(intersection.point, player::pos);
      distance = distance * cosf(rayAngle - player::rotRad); // fisheye correction

      intersectedWalls.push_back({ray, distance, intersection.where, &wall});
    }
  }
  sort(intersectedWalls.begin(), intersectedWalls.end());
  for (size_t i = 0; i < intersectedWalls.size(); i++) {
    const WallSlice *wallSlice = &intersectedWalls[i];
    DrawWallSlice(*wallSlice);
  }
}

void ray_caster::CastRays() {

  const float fov = static_cast<float>(display::width) / static_cast<float>(display::height);
  const float startAngle = player::rotRad - fov / 2.0f;
  const float angleStep = fov / static_cast<float>(display::width);

  if (multiThreaded) {
    const int threadCount = static_cast<int>(thread::hardware_concurrency());
    vector<thread> threads;
    const int segmentSize = display::width / threadCount;

    for (int t = 0; t < threadCount; t++) {
      const int start = t * segmentSize;
      const int end = start + segmentSize;

      threads.emplace_back([=] {
        for (int ray = start; ray < end; ray++) {
          CastRay(ray, startAngle, angleStep);
        }
      });
    }

    for (auto &t : threads) {
      t.join();
    }
  } else {
    for (int ray = 0; ray < display::width; ray++) {
      CastRay(ray, startAngle, angleStep);
    }
  }
}