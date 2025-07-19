#ifndef GEOMETRY_H
#define GEOMETRY_H
#include <cfloat>
#include <cmath>
#include <cstdint>

namespace geometry {
struct Vector2i {
  int x, y;

  Vector2i() = default;
  Vector2i(const int x, const int y) : x(x), y(y) {}

  Vector2i operator+(const Vector2i &other) const { return {x + other.x, y + other.y}; }
  Vector2i operator-(const Vector2i &other) const { return {x - other.x, y - other.y}; }
};

struct Vector2 {
  float x, y;

  Vector2() = default;
  Vector2(const float x, const float y) : x(x), y(y) {}

  explicit operator Vector2i() const {
    const int rounded_x = static_cast<int>(x);
    const int rounded_y = static_cast<int>(y);
    return {rounded_x, rounded_y};
  }

  Vector2 operator+(const Vector2 &other) const { return {x + other.x, y + other.y}; }
  Vector2 operator-(const Vector2 &other) const { return {x - other.x, y - other.y}; }
  constexpr float Length() const { return x * x + y * y; }
  constexpr float LengthSquared() const { return std::sqrt(Length()); }
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

typedef struct {
  Vector2 point;
  float where;
} Intersection;

constexpr float lerpf(const float from, const float to, const float percentage) { return from + (to - from) * percentage; }

constexpr float deg2rad(const float num) { return num * (static_cast<float>(M_PI) / 180.0f); }

constexpr float rad2deg(const float num) { return num * 57.29578f; }

inline Vector2 Normalize(const Vector2 &v) {
  const float length = v.LengthSquared();
  if (length == 0.0f) {
    return {0.0f, 0.0f}; // so won't divide by zero
  }
  return {v.x / length, v.y / length};
}

inline float EuclideanDistance(const Vector2 &from, const Vector2 &to) {
  const Vector2 dir = to - from;
  return dir.LengthSquared();
}

constexpr float DotProduct(const Vector2 &a, const Vector2 &b) { return a.x * b.x + a.y * b.y; }

inline Vector2 GetForwardVector(const float rotRad) { return {cosf(rotRad), sinf(rotRad)}; }

inline bool IsFacingTarget(const Vector2 &fromPos, const Vector2 &forwardVector, const Vector2 &targetPos, const float limit) {
  const float dotProduct = DotProduct(forwardVector, Normalize(targetPos - fromPos));

  if (dotProduct > limit) {
    return true;
  }

  return false;
}

inline Intersection LineIntersection(const float x1, const float y1, const float x2, const float y2, const float x3, const float y3, const float x4,
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
} // namespace geometry

#endif
