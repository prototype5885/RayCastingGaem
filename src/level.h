#ifndef LEVEL_H
#define LEVEL_H

#include "geometry.h"

#include <string>
#include <vector>

using namespace std;

namespace level {
class Wall {
public:
  geometry::Vector2 from, to;
  float wallLength;
  string textureBottom, textureMid, textureTop;
  bool collision;

  bool operator==(const Wall &wall) const { return from == wall.from && to == wall.to; }
  bool operator!=(const Wall &wall) const { return !(from == wall.from && to == wall.to); }
};

class Sector {
public:
  float bottom, top;
  vector<Wall> walls;
};

class Level {
public:
  string name;
  vector<Sector> sectors;
};

extern Level currentLevel;
void LoadLevel(const string &name);
// geometry::Vector2 GetMapDimension();
} // namespace level

#endif