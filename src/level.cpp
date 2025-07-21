#include "level.h"
#include "texture.h"

#include <cfloat>
#include <cstdint>
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
    throw runtime_error("Could not open level file " + filePath);
  }

  currentLevel.sectors.clear();

  set<string> wallTextures;

  string line;
  int lineCounter = 0;
  while (getline(file, line)) {
    istringstream iss(line);

    if (line.empty() || line.at(0) == '#') {
      continue;
    }

    char typeIdentifier;
    iss >> typeIdentifier;

    if (typeIdentifier == 's') {
      currentLevel.sectors.push_back(Sector{});
      Sector &sector = currentLevel.sectors.back();
      if (iss >> sector.bottom >> sector.top) {
      } else {
        throw runtime_error("Failed parsing level file " + filePath + ", error at line " + to_string(lineCounter) + "\n");
      }
    } else if (typeIdentifier == 'w') {
      currentLevel.sectors.back().walls.push_back(Wall{});
      Wall &wall = currentLevel.sectors.back().walls.back();

      if (iss >> wall.a >> wall.b >> wall.c >> wall.d >> wall.textureBottom >> wall.textureMid >> wall.textureTop >> wall.collision) {
      } else {
        throw runtime_error("Failed parsing level file " + filePath + ", error at line " + to_string(lineCounter) + "\n");
      }

      wall.wallLength = geometry::EuclideanDistance({wall.a, wall.b}, {wall.c, wall.d});

      wallTextures.insert(wall.textureBottom);
      wallTextures.insert(wall.textureMid);
      wallTextures.insert(wall.textureTop);
    } else if (typeIdentifier == 'e') {
      vector<Wall> &walls = currentLevel.sectors.back().walls;
      if (walls.front().a != walls.back().c || walls.front().b != walls.back().d) {
        char errorMessage[128];
        snprintf(errorMessage, sizeof(errorMessage), "Sector is not enclosed, starts at (%f, %f), ends at (%f, %f)\n", walls.front().a,
                 walls.front().b, walls.back().c, walls.back().d);
        throw runtime_error(errorMessage);
      }
    }

    lineCounter++;
  }
  currentLevel.name = name;
  texture::LoadTextures(wallTextures);

  printf("Successfully loaded %zu sectors from level %s\n", currentLevel.sectors.size(), name.c_str());
}

// geometry::Vector2 GetMapDibmension() {
//   using namespace geometry;
//
//   float minX = FLT_MAX;
//   float maxX = FLT_MIN;
//   float minY = FLT_MAX;
//   float maxY = FLT_MIN;
//
//   const vector<Wall> &walls = currentLevel.walls;
//   for (size_t i = 0; i < walls.size(); i++) {
//     const Wall &wall = walls[i];
//
//     if (wall.a < minX)
//       minX = wall.a;
//     else if (wall.a > maxX)
//       maxX = wall.a;
//
//     if (wall.b < minY)
//       minY = wall.b;
//     else if (wall.b > maxY)
//       maxY = wall.b;
//
//     if (wall.c < minX)
//       minX = wall.c;
//     else if (wall.c > maxX)
//       maxX = wall.c;
//
//     if (wall.d < minY)
//       minY = wall.d;
//     else if (wall.d > maxY)
//       maxY = wall.d;
//   }
//
//   const Vector2 dimension = {maxX - minX, maxY - minY};
//
//   printf("%f%f\n", dimension.x, dimension.y);
//
//   return dimension;
// }
} // namespace level