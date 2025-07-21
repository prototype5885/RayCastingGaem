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
  string textureBottom, textureMid, textureTop;
  bool collision;
} Wall;

typedef struct {
  float bottom, top;
  vector<Wall> walls;
} Sector;

typedef struct {
  string name;
  vector<Sector> sectors;
} Level;

extern Level currentLevel;
void LoadLevel(const string &name);
// geometry::Vector2 GetMapDimension();
} // namespace level

#endif