#include "level.h"
#include "texture.h"

#include <cfloat>
#include <filesystem>
#include <format>
#include <fstream>
#include <iostream>
#include <set>
#include <string>
#include <vector>

using namespace std;
using namespace filesystem;

namespace level {
vector<uint8_t> currentLevel;

int LoadLevel(string name) {
  cout << format("Loading level {}...\n", name);

  const path filePath = "assets/levels/" + name + ".txt";
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

  cout << format("Loaded level {}, bytes: {}\n", name, currentLevel.size());
  texture::LoadTextures(wallTypes);

  return 0;
}

geometry::Vector2 GetMapDimension() {
  using namespace geometry;

  float minX = FLT_MAX;
  float maxX = FLT_MIN;
  float minY = FLT_MAX;
  float maxY = FLT_MIN;

  for (auto [x1, y1, x2, y2] : walls) {
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

  Vector2 dimension = {maxX - minX, maxY - minY};

  cout << format("{}x{}", dimension.x, dimension.y) << endl;

  return dimension;
}
} // namespace level