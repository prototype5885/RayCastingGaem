#include "level.h"
#include "filesystem.h"
#include "texture.h"

#include <cfloat>
#include <fstream>
#include <iostream>
#include <set>
#include <vector>

using namespace std;

namespace level {
vector<uint8_t> currentLevel;

int LoadLevel(const char *name) {
  printf("Loading level %s\n", name);

  char filePath[MAX_FILEPATH_LENGTH];
  snprintf(filePath, MAX_FILEPATH_LENGTH, "assets/levels/%s.txt", name);

  ifstream file(filePath);
  if (!file.good()) {
    cerr << "Couldn't find level " << filePath << endl;
    return 1;
  }

  currentLevel.clear();

  set<uint8_t> wallTypes;
  uint8_t ch;
  while (file >> ch) {
    if (ch != '\n') {
      auto wallType = static_cast<uint8_t>(ch - '0');
      currentLevel.push_back(wallType);
      if (wallType != 0) {
        wallTypes.insert(wallType);
      }
    }
  }

  printf("Loaded level %s, bytes: %zu\n", name, currentLevel.size());

  texture::LoadTextures(wallTypes);

  return 0;
}

geometry::Vector2 GetMapDimension() {
  using namespace geometry;

  float minX = FLT_MAX;
  float maxX = FLT_MIN;
  float minY = FLT_MAX;
  float maxY = FLT_MIN;

  for (size_t i = 0; i < walls.size(); i++) {
    const float x1 = walls[i].a;
    const float y1 = walls[i].b;
    const float x2 = walls[i].c;
    const float y2 = walls[i].d;

    if (x1 < minX)
      minX = x1;
    else if (x1 > maxX)
      maxX = x1;

    if (y1 < minY)
      minY = y1;
    else if (y1 > maxY)
      maxY = y1;

    if (x2 < minX)
      minX = x2;
    else if (x2 > maxX)
      maxX = x2;

    if (y2 < minY)
      minY = y2;
    else if (y2 > maxY)
      maxY = y2;
  }

  const Vector2 dimension = {maxX - minX, maxY - minY};

  printf("%f%f\n", dimension.x, dimension.y);

  return dimension;
}
} // namespace level