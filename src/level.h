#ifndef LEVEL_H
#define LEVEL_H

#include "filesystem.h"
#include "geometry.h"

#define MAX_WALLS_IN_SECTOR 64
#define MAX_SECTORS_ON_LEVEL 1024

using namespace std;

namespace level {
struct Wall {
  geometry::Vector2 from, to;
  float wallLength;
  char textureBottom[MAX_FILENAME_LENGTH], textureMid[MAX_FILENAME_LENGTH], textureTop[MAX_FILENAME_LENGTH];
  bool collision;

  bool operator==(const Wall &wall) const { return from == wall.from && to == wall.to; }
  bool operator!=(const Wall &wall) const { return !(from == wall.from && to == wall.to); }
};

typedef struct {
  float bottom, top;
  Wall walls[MAX_WALLS_IN_SECTOR];
  uint8_t wallCount;
  // vector<Wall> walls;
} Sector;

typedef struct {
  char name[MAX_FILENAME_LENGTH];
  Sector sectors[MAX_SECTORS_ON_LEVEL];
  uint8_t sectorCount;
  // vector<Sector> sectors;
} Level;

extern Level currentLevel;
void LoadLevel(const char *name);
// geometry::Vector2 GetMapDimension();
} // namespace level

#endif