#ifndef LEVEL_H
#define LEVEL_H

#include "geometry.h"

#include <string>
#include <vector>

using namespace std;

namespace level {
typedef struct {
  float a, b, c, d;
  float wallLength;
  string texture;
} Wall;

struct Level {
  string name;
  vector<Wall> walls;
};

extern Level currentLevel;
void LoadLevel(const string &name);
geometry::Vector2 GetMapDimension();
} // namespace level

#endif