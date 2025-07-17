#ifndef LEVEL_H
#define LEVEL_H

#include "geometry.h"

#include <cstdint>
#include <string>
#include <vector>

using namespace std;

namespace level {
struct Wall {
  float a, b, c, d;
  float wallLength;
  uint16_t texture;
};

struct Level {
  string name;
  vector<Wall> walls;
};

extern Level currentLevel;
void LoadLevel(const string &name);
geometry::Vector2 GetMapDimension();
} // namespace level

#endif