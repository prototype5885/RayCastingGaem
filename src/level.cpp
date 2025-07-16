#include "level.h"

#include <cfloat>
#include <fstream>
#include <iostream>
#include <sstream>

using namespace std;

namespace level {
Level currentLevel;

void LoadLevel(const string &name) {
  printf("Loading level %s\n", name.c_str());

  string filePath = "assets/levels/" + name + ".txt";
  ifstream file(filePath);

  if (!file.is_open()) {
    throw(runtime_error("Could not open file " + filePath));
  }

  currentLevel.walls.clear();

  string line;
  int counter = 1;
  while (getline(file, line)) {
    istringstream iss(line);

    currentLevel.walls.push_back(Wall{});

    if (iss >> currentLevel.walls.back().a >> currentLevel.walls.back().b >> currentLevel.walls.back().c >> currentLevel.walls.back().d >>
        currentLevel.walls.back().texture) {
      string remaining;
      if (iss >> remaining) {
        printf("There was extra data on line %d\n", counter);
      }
    } else {
      throw(runtime_error("Failed parsing map file " + filePath + ", error at line " + to_string(counter) + "\n"));
    }
    counter++;
  }

  printf("Successfully loaded %llu walls from level %s\n", currentLevel.walls.size(), name.c_str());
}

geometry::Vector2 GetMapDimension() {
  using namespace geometry;

  float minX = FLT_MAX;
  float maxX = FLT_MIN;
  float minY = FLT_MAX;
  float maxY = FLT_MIN;

  const vector<Wall> &walls = currentLevel.walls;
  for (size_t i = 0; i < walls.size(); i++) {
    const Wall &wall = walls[i];
    const float x1 = wall.a;
    const float y1 = wall.b;
    const float x2 = wall.c;
    const float y2 = wall.d;

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