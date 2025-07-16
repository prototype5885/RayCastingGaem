#ifndef LEVEL_H
#define LEVEL_H

#include "geometry.h"

#include <cstdint>
#include <vector>

using namespace std;

namespace level {
struct Wall {
  float a, b, c, d;
  uint8_t texture;
};

// clang-format off
const vector<Wall> walls = {
  {5, 5, 50, 5, 0},
  {50, 5, 50, 50, 0},
  {50, 50, 5, 50, 0},
  {5, 50, 5, 5, 0},
  {20, 10, 20, 40, 1},
  {30, 10, 40, 20, 0},
  {30, 40, 40, 30, 2},
  {25, 20, 35, 20, 0},
  {35, 30, 45, 30, 0},
  {45, 30, 45, 45, 0},
  {45, 45, 35, 45 ,0}
};
// clang-format on

extern vector<uint8_t> currentLevel;
int LoadLevel(const char *name);
geometry::Vector2 GetMapDimension();
} // namespace level

#endif