#ifndef GEOMETRY_H
#define GEOMETRY_H
#include <cstdint>

namespace geometry {

struct Vector2i {
  int x, y;

  Vector2i operator+(const Vector2i &other) const { return Vector2i(x + other.x, y + other.y); }
  Vector2i operator-(const Vector2i &other) const { return Vector2i(x - other.x, y - other.y); }
};

struct Vector2 {
  float x, y;

  explicit operator Vector2i() const {
    const int rounded_x = static_cast<int>(x);
    const int rounded_y = static_cast<int>(y);
    return Vector2i(rounded_x, rounded_y);
  }

  Vector2 operator+(const Vector2 &other) const { return Vector2(x + other.x, y + other.y); }
  Vector2 operator-(const Vector2 &other) const { return Vector2(x - other.x, y - other.y); }
};

typedef struct {
  int8_t x, y;
} Vector2i8;

typedef struct {
  float a;
  float b;
  float c;
  float d;
} Line2D;

float lerpf(float from, float to, float percentage);
float deg2rad(float num);
float rad2deg(float num);
// Vector2 LinesIntersectionPoint(Vector2 a, Vector2 b, Vector2 c, Vector2 d);
float CalculateRayDistance(Vector2 from, Vector2 to);
void PrintVector2(Vector2 vec);
} // namespace geometry

#endif
