#ifndef EXTRAMATH_H
#define EXTRAMATH_H
#include <cstdint>

typedef struct {
  float x, y;
} Vector2;

typedef struct {
  int x, y;
} Vector2i;

typedef struct {
  int8_t x, y;
} Vector2i8;

float lerpf(float from, float to, float percentage);
float deg2rad(float num);
float rad2deg(float num);
int clampi(int d, int min, int max);

#endif
