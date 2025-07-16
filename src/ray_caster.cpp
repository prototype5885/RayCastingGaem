#include "ray_caster.h"
#include "colors.h"
#include "display.h"
#include "geometry.h"
#include "level.h"
#include "map_view.h"
#include "player.h"

#include <algorithm>
#include <cfloat>
#include <cmath>
#include <cstdint>
#include <iostream>
#include <vector>

using namespace std;
using namespace geometry;

typedef struct {
  float minDistance;
  Vector2 hitPoint;
  uint16_t texture;
} RayHitPoint;

Vector2 LineIntersection(const float x1, const float y1, const float x2, const float y2, const float x3, const float y3, const float x4,
                         const float y4) {
  const float den = (x1 - x2) * (y3 - y4) - (y1 - y2) * (x3 - x4);
  if (den == 0)
    return {FLT_MAX, FLT_MAX};
  const float t = ((x1 - x3) * (y3 - y4) - (y1 - y3) * (x3 - x4)) / den;
  const float u = -((x1 - x2) * (y1 - y3) - (y1 - y2) * (x1 - x3)) / den;
  if (t >= 0 && t <= 1 && u >= 0 && u <= 1) {
    const float px = x1 + t * (x2 - x1);
    const float py = y1 + t * (y2 - y1);
    return {px, py};
  }
  return {FLT_MAX, FLT_MAX};
}

float PointToLineDistance(const float px, const float py, const float x1, const float y1, const float x2, const float y2) {
  const float dx = x2 - x1;
  const float dy = y2 - y1;
  const float lenSquared = powf(dx, 2) + powf(dy, 2);
  if (lenSquared == 0) {
    return CalculateRayDistance({px, py}, {x1, y1});
  }
  float t = ((px - x1) * dx + (py - y1) * dy) / lenSquared;
  t = max(0.0f, min(1.0f, t));
  const float projX = x1 + t * dx;
  const float projY = y1 + t * dy;
  return CalculateRayDistance({px, py}, {projX, projY});
}

RayHitPoint CastRay(const float rayAngle) {
  using namespace level;

  float minEuclideanDistance = FLT_MAX;
  Vector2 closestHitPoint = {FLT_MAX, FLT_MAX};
  uint16_t hitWallTexture = 0;

  const float rayX = cosf(rayAngle);
  const float rayY = sinf(rayAngle);

  // 1. Find the CLOSEST wall by comparing TRUE distances
  // for (auto [x1, y1, x2, y2] : level::walls) {
  for (size_t i = 0; i < currentLevel.walls.size(); i++) {
    const float x1 = currentLevel.walls[i].a;
    const float y1 = currentLevel.walls[i].b;
    const float x2 = currentLevel.walls[i].c;
    const float y2 = currentLevel.walls[i].d;

    const Vector2 intersection =
        LineIntersection(player::pos.x, player::pos.y, player::pos.x + rayX * 1000, player::pos.y + rayY * 1000, x1, y1, x2, y2);

    if (intersection.x != FLT_MAX) {
      // Calculate the true, uncorrected distance
      const float euclideanDist = CalculateRayDistance(intersection, player::pos);

      // Compare true distances to find the actual closest wall
      if (euclideanDist < minEuclideanDistance) {
        minEuclideanDistance = euclideanDist;
        closestHitPoint = intersection;
        hitWallTexture = currentLevel.walls[i].texture;
      }
    }
  }

  // 2. If a wall was hit, apply fisheye correction ONCE to the final distance
  if (minEuclideanDistance != FLT_MAX) {
    const float correctedDistance = minEuclideanDistance * cosf(rayAngle - player::rotRad);
    return {correctedDistance, closestHitPoint, hitWallTexture};
  }

  // 3. If no walls were hit, return the "no hit" value
  return {FLT_MAX, {FLT_MAX, FLT_MAX}, hitWallTexture};
}

void DrawWallSlice(const int x, const float distance, const uint16_t texture) {
  const int wallHeight = static_cast<int>(static_cast<float>(display::height) / distance); // this is how tall the wall will be based on ray
  const int middle = display::height / 2;

  int startPos = middle - wallHeight / 2; // wall starts at this height
  startPos = static_cast<int>(static_cast<float>(startPos) + player::rotVerticalRad);
  if (startPos < 0) // prevent it from starting from above the screen
    startPos = 0;

  int endPos = middle + wallHeight / 2; // wall ends here
  endPos = static_cast<int>(static_cast<float>(endPos) + player::rotVerticalRad);
  if (endPos > display::height) // prevent it from starting from below the screen
    endPos = display::height;

  constexpr float minPercentage = 1.0f;
  constexpr float maxPercentage = 64.0f;

  float percentage = 1.0f - (distance - minPercentage) / (maxPercentage - minPercentage);

  if (distance < minPercentage) {
    percentage = 1.0f;
  } else if (distance > maxPercentage) {
    percentage = 0.0f;
  }

  if (percentage < 0.25f)
    percentage = 0.25f;

  uint32_t color = WHITE_COLOR;
  if (texture == 1) {
    color = RED_COLOR;
  } else if (texture == 2) {
    color = GREEN_COLOR;
  } else if (texture == 3) {
    color = BLUE_COLOR;
  }
  color = color::MultiplyRGB(color, percentage);

  for (int y = startPos; y < endPos; y++) {
    // float t = static_cast<float>(y - startPos) / endPos - startPos;
    // t = std::max(0.0f, std::min(1.0f, t));
    // float currentBrightness = 0.2f + (1.0f - 0.2f) * t;

    // const int verticalSegment = static_cast<int>(textureDimension * horizontalHitPoint);

    // int hpi = static_cast<int>(horizontalSegment) * texture->height + verticalSegment;
    // hpi = clampi(hpi, 0, texture->width * texture->height - 1);

    // horizontalSegment += pixelColumnOnEachRay;

    // const uint8_t color = texture->colors.at(hpi);

    // uint32_t color = texture->colors.at(hpi);

    // color = color::MultiplyRGB(color, currentBrightness);
    // AddPixelToBufferUnsafe(ray, y, color);
    // AddPixelToBuffer(ray, y, color);
    display::AddPixelToBuffer(x, y, color);
  }
}

void ray_caster::CastRays() {
  const float fov = static_cast<float>(display::width) / static_cast<float>(display::height);
  const float startAngle = player::rotRad - fov / 2.0f;
  const float angleStep = fov / static_cast<float>(display::width);

  for (int ray = 0; ray < display::width; ray++) {
    const float currentAngle = startAngle + static_cast<float>(ray) * angleStep;

    const RayHitPoint rhp = CastRay(currentAngle);

    if (rhp.hitPoint.x != FLT_MAX || rhp.hitPoint.y != FLT_MAX) {
      if (map_view::mapView) {
        map_view::DrawRay(rhp.hitPoint);
      } else {
        DrawWallSlice(ray, rhp.minDistance, rhp.texture);
      }
    }
  }
}