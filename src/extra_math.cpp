#include <cmath>

float lerpf(const float from, const float to, const float percentage) { return from + (to - from) * percentage; }

float deg2rad(const float num) { return num * (static_cast<float>(M_PI) / 180.0f); }

float rad2deg(const float num) { return num * 57.29578f; }

int clampi(const int d,const  int min,const  int max) {
  const int t = d < min ? min : d;
  return t > max ? max : t;
}
