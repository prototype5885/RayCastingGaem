#ifndef LEVEL_H
#define LEVEL_H

#include "geometry.h"

#include <string>
#include <vector>

namespace level {
class Wall {
public:
  geometry::Vector2 from, to;
  float wallLength;
  std::string textureBottom, textureMid, textureTop;
  bool collision;

  bool operator==(const Wall &wall) const { return from == wall.from && to == wall.to; }
  bool operator!=(const Wall &wall) const { return !(from == wall.from && to == wall.to); }
};

class Sector {
public:
  float bottom, top;
  std::vector<Wall> walls;
};

class Level {
public:
  std::string name;
  std::vector<Sector> sectors;
};

extern Level currentLevel;
void LoadLevel(const std::string &name);
// geometry::Vector2 GetMapDimension();
} // namespace level

#endif