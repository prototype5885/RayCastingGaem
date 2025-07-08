#include <cmath>

float lerpf(float from, float to, float percentage) { return from + (to - from) * percentage; }

float deg2rad(float num) { return num * (M_PI / 180.0); }

float rad2deg(float num) { return num * 57.29578; }

int clampi(int d, int min, int max) {
  const int t = d < min ? min : d;
  return t > max ? max : t;
}
