#ifndef GEOMETRY_H
#define GEOMETRY_H
#include "glm/vec2.hpp"
#include <glm/geometric.hpp>

#include <cfloat>

namespace geometry {
class Intersection {
public:
  glm::vec2 point;
  float where;
};

constexpr float lerpf(const float from, const float to, const float percentage) { return from + (to - from) * percentage; }

inline glm::vec2 GetForwardVector(const float rotRad) { return glm::vec2{cosf(rotRad), sinf(rotRad)}; }

inline float IsFacingTarget(const glm::vec2 fromPos, const glm::vec2 forwardVector, const glm::vec2 targetPos) {
  const glm::vec2 toPoint = targetPos - fromPos;
  const glm::vec2 toPointDir = glm::normalize(toPoint);
  return glm::dot(forwardVector, toPointDir);
}

inline Intersection LineIntersection(const glm::vec2 f1, const glm::vec2 t1, const glm::vec2 f2, const glm::vec2 t2) {
  const float den = (f1.x - t1.x) * (f2.y - t2.y) - (f1.y - t1.y) * (f2.x - t2.x);
  if (den == 0)
    return {{FLT_MAX, FLT_MAX}, 0.0f};
  const float t = ((f1.x - f2.x) * (f2.y - t2.y) - (f1.y - f2.y) * (f2.x - t2.x)) / den;
  const float u = -((f1.x - t1.x) * (f1.y - f2.y) - (f1.y - t1.y) * (f1.x - f2.x)) / den;
  if (t >= 0 && t <= 1 && u >= 0 && u <= 1) {
    const float px = f1.x + t * (t1.x - f1.x);
    const float py = f1.y + t * (t1.y - f1.y);
    return {{px, py}, u};
  }
  return {{FLT_MAX, FLT_MAX}, 0.0f};
}
} // namespace geometry

#endif
