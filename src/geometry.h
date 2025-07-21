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
  bool operator==(const Vector2 &other) const { return x == other.x && y == other.y; }
  bool operator!=(const Vector2 &other) const { return !(*this == other); }
  constexpr float Length() const { return x * x + y * y; }
  constexpr float LengthSquared() const { return std::sqrt(Length()); }
  bool IsInfinite() const { return Vector2{x, y} == Vector2{FLT_MAX, FLT_MAX}; }
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

inline Intersection LineIntersection(const Vector2 f1, const Vector2 t1, const Vector2 f2, const Vector2 t2) {
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
