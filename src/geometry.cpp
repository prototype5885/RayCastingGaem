#include "geometry.h"

#include <cfloat>
#include <cmath>
#include <iostream>

namespace geometry {
float lerpf(const float from, const float to, const float percentage) { return from + (to - from) * percentage; }
float deg2rad(const float num) { return num * (static_cast<float>(M_PI) / 180.0f); }
float rad2deg(const float num) { return num * 57.29578f; }

// Vector2 LinesIntersectionPoint(const Vector2 a, const Vector2 b, const Vector2 c, const Vector2 d) {
//   // line 1
//   const float a1 = b.y - a.y;
//   const float b1 = a.x - b.x;
//   const float c1 = a1 * a.x + b1 * a.y;
//
//   // line 2
//   const float a2 = d.y - c.y;
//   const float b2 = c.x - d.x;
//   const float c2 = a2 * c.x + b2 * c.y;
//
//   const float determinant = a1 * b2 - a2 * b1;
//   if (determinant == 0) {
//     return Vector2(FLT_MAX, FLT_MAX); // lines are parallel
//   }
//   float x = (b2 * c1 - b1 * c2) / determinant;
//   float y = (a1 * c2 - a2 * c1) / determinant;
//   return Vector2(x, y);
// }

float EuclideanDistance(const Vector2 from, const Vector2 to) {
  const float dx = from.x - to.x;
  const float dy = from.y - to.y;
  return sqrtf(dx * dx + dy * dy);
}

void PrintVector2(const Vector2 vec) { printf("(%f, %f)\n", vec.x, vec.y); }
} // namespace geometry
