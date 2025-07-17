#include "ray_caster.h"
#include "colors.h"
#include "display.h"
#include "geometry.h"
#include "level.h"
#include "map_view.h"
#include "player.h"
#include "texture.h"
#include "utils.h"

#include <algorithm>
#include <cfloat>
#include <cmath>
#include <cstdint>
#include <io.h>
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

RayHitPoint CastRay(const float rayAngle) {
  using namespace level;

  float minEuclideanDistance = FLT_MAX;
  Vector2 closestHitPoint = {FLT_MAX, FLT_MAX};
  string hitWallTexture;
  float hitPart = 0.0f;
  float wallLength = 0.0f;

  const float rayX = cosf(rayAngle);
  const float rayY = sinf(rayAngle);

  // 1. Find the CLOSEST wall by comparing TRUE distances
  const vector<Wall> &walls = currentLevel.walls;
  for (size_t i = 0; i < currentLevel.walls.size(); i++) {
    const Wall &wall = walls[i];

    const Intersection intersection = LineIntersection(player::pos.x, player::pos.y, player::pos.x + rayX * MAX_RAY_DISTANCE,
                                                       player::pos.y + rayY * MAX_RAY_DISTANCE, wall.a, wall.b, wall.c, wall.d);

    if (intersection.point.x != FLT_MAX) {
      // Calculate the true, uncorrected distance
      const float euclideanDist = EuclideanDistance(intersection.point, player::pos);

      // Compare true distances to find the actual closest wall
      if (euclideanDist < minEuclideanDistance) {
        minEuclideanDistance = euclideanDist;
        closestHitPoint = intersection.point;
        hitWallTexture = currentLevel.walls[i].texture;
        hitPart = intersection.part;
        wallLength = wall.wallLength;
      }
    }
  }

  // 2. If a wall was hit, apply fisheye correction ONCE to the final distance
  if (minEuclideanDistance != FLT_MAX) {
    const float correctedDistance = minEuclideanDistance * cosf(rayAngle - player::rotRad);
    return {correctedDistance, closestHitPoint, hitPart, hitWallTexture, wallLength};
  }

  // 3. If no walls were hit, return the "no hit" value
  return {FLT_MAX, {FLT_MAX, FLT_MAX}, hitPart, hitWallTexture, wallLength};
}

void DrawWallSlice(const int wallX, const float distance, const float part, const string &wallTexture, const float wallLength) {
  const int wallHeight = static_cast<int>(static_cast<float>(display::height) / distance); // this is how tall the wall will be based on ray
  const int middle = display::height / 2;

  int startPos = middle - wallHeight / 2; // wall starts at this height
  startPos = static_cast<int>(static_cast<float>(startPos) + player::rotVerticalRad);
  // commented out as this is not needed because it won't render off screen anyway and this causes texture distortion
  // if (startPos < 0) // prevent it from starting from above the screen
  //   startPos = 0;

  int endPos = middle + wallHeight / 2; // wall ends here
  endPos = static_cast<int>(static_cast<float>(endPos) + player::rotVerticalRad);
  // commented out as this is not needed because it won't render off screen anyway and this causes texture distortion
  // if (endPos > display::height) // prevent it from starting from below the screen
  //   endPos = display::height;

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

  texture::Texture *texture;
  try {
    texture = &texture::textureList.at(wallTexture);
  } catch (exception &e) {
    texture = &texture::missingTexture;
  }

  // const float textureDimension = fminf(texture->width, texture->height);
  int textureX = static_cast<int>(static_cast<float>(texture->width) * part * wallLength);
  textureX = textureX % texture->width;
  textureX = utils::clamp(textureX, 0, texture->height - 1);

  const float textureYstep = static_cast<float>(texture->height) / static_cast<float>(wallHeight);

  for (int wallY = startPos; wallY < endPos; wallY++) {
    int textureY = static_cast<int>(static_cast<float>((wallY - startPos)) * textureYstep);
    textureY = utils::clamp(textureY, 0, texture->height - 1);

    const int pos = textureY * texture->width + textureX;
    const uint32_t color = texture->colors.at(pos);

    display::AddPixelToBuffer(wallX, wallY, color);
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
        DrawWallSlice(ray, rhp.minDistance, rhp.part, rhp.texture, rhp.wallLength);
      }
    }
  }
}