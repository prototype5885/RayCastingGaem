#include <math.h>
#include <stdlib.h>
// #define ONE_RAD (3.14159 / 180.0);

float lerpf(float from, float to, float percentage)
{
  return from + (to - from) * percentage;
}

float deg2rad(float num) { return num * (M_PI / 180.0); }

float rad2deg(float num) { return num * 57.29578; }

float sqr(float num) { return num * num; }

int clampi(int d, int min, int max)
{
  const int t = d < min ? min : d;
  return t > max ? max : t;
}

// int RandomInRange(int min, int max)
// {
//   return (random() % (max - min + 1)) + min;
// }

int RandomMax(int max)
{
  return random() % max;
}