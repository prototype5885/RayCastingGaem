#include "level.h"
#include "filesystem.h"
#include "texture.h"

#include <cfloat>
#include <cstdint>
#include <cstring>
#include <fstream>
#include <iostream>
#include <sstream>

using namespace std;

namespace level {
Level currentLevel{};

void LoadLevel(const char *name) {
  printf("Loading level %s\n", name);

  char filePath[MAX_FILEPATH_LENGTH];
  snprintf(filePath, sizeof(filePath), "assets/levels/%s.txt", name);

  // string filePath = "assets/levels/" + name + ".txt";
  ifstream file(filePath);

  if (!file.is_open()) {
    char errorText[64];
    snprintf(errorText, sizeof(errorText), "Could not open file %s", filePath);
    throw runtime_error(errorText);
  }

  // currentLevel.sectors.clear();
  currentLevel.sectorCount = 0;
  uint8_t &sectorIndex = currentLevel.sectorCount;
  uint8_t wallIndex = 0;
  uint16_t totalWalls = 0;

  set<string> wallTextures;

  string line;
  uint16_t lineCounter = 0;
  while (getline(file, line)) {
    istringstream iss(line);

    if (line.empty() || line.at(0) == '#') {
      continue;
    }

    char typeIdentifier;
    iss >> typeIdentifier;

    if (typeIdentifier == 's') { // if sector
      wallIndex = 0;

      Sector &sector = currentLevel.sectors[sectorIndex];
      Wall &wall = sector.walls[wallIndex];

      // sets the starting point for the first wall
      if (iss >> wall.from.x >> wall.from.y >> sector.bottom >> sector.top) {
      } else {
        char errorMessage[128];
        snprintf(errorMessage, sizeof(errorMessage), "Failed parsing level file %s, error at line %u\n", filePath, lineCounter);
        throw runtime_error(errorMessage);
      }

    } else if (typeIdentifier == 'w') { // if wall
      Sector &sector = currentLevel.sectors[sectorIndex];
      Wall &wall = sector.walls[wallIndex];

      // sets the target point for the current wall
      if (iss >> wall.to.x >> wall.to.y >> wall.textureBottom >> wall.textureMid >> wall.textureTop >> wall.collision) {
      } else {
        char errorMessage[128];
        snprintf(errorMessage, sizeof(errorMessage), "Failed parsing level file %s, error at line %u\n", filePath, lineCounter);
        throw runtime_error(errorMessage);
      }

      // don't need to run this if first wall because it has been already set in first 's'
      if (wallIndex != 0) {
        // grab the end point from the previous wall to use as the new wall's starting point
        wall.from.x = currentLevel.sectors[sectorIndex].walls[wallIndex - 1].to.x;
        wall.from.y = currentLevel.sectors[sectorIndex].walls[wallIndex - 1].to.y;
      }

      // check if the wall intersects with any other walls
      // for (size_t s = 0; s < currentLevel.sectorCount; s++) {
      //   for (size_t w = 0; w < currentLevel.sectors.at(s).walls.size(); w++) {
      //     Wall &wallToCompareWith = currentLevel.sectors.at(s).walls.at(w);
      //     if (wall == wallToCompareWith) {
      //       continue;
      //     }
      //     geometry::Intersection intersection = geometry::LineIntersection(wall.from, wall.to, wallToCompareWith.from, wallToCompareWith.to);
      //     if (!intersection.point.IsInfinite()) {
      //       cout << "intersects" << endl;
      //       // throw runtime_error("Intersects");
      //       char errorMessage[128];
      //       snprintf(errorMessage, sizeof(errorMessage), "Wall (%f, %f, %f, %f) intersects with wall (%f, %f, %f, %f)\n", wall.from.x, wall.from.y,
      //                wall.to.x, wall.to.y, wallToCompareWith.from.x, wallToCompareWith.from.y, wallToCompareWith.to.x, wallToCompareWith.to.y);
      //       throw runtime_error(errorMessage);
      //     }
      //   }
      // }

      wall.wallLength = geometry::EuclideanDistance({wall.from.x, wall.from.y}, {wall.to.x, wall.to.y});

      // currentLevel.sectors.back().walls.push_back(wall);

      wallTextures.insert(wall.textureBottom);
      wallTextures.insert(wall.textureMid);
      wallTextures.insert(wall.textureTop);

      wallIndex++;
      sector.wallCount++;
      totalWalls++;
    } else if (typeIdentifier == 'e') { // if sector end
      // check if the first
      const Wall *walls = currentLevel.sectors[sectorIndex].walls;
      // needs to have -1 because wallIndex was incremented in the previous line expecting a new wall
      if (walls[0].from != walls[wallIndex - 1].to) {
        char errorMessage[128];
        snprintf(errorMessage, sizeof(errorMessage), "Sector is not enclosed, starts at (%f, %f), ends at (%f, %f)\n", walls[0].from.x,
                 walls[0].from.y, walls[wallIndex - 1].to.x, walls[wallIndex - 1].to.y);
        throw runtime_error(errorMessage);
      }
      sectorIndex++;
      wallIndex = 0;
    }

    lineCounter++;
  }
  snprintf(currentLevel.name, 16, name);
  texture::LoadTextures(wallTextures);

  printf("Successfully loaded %u/%u sectors with %u walls from level %s\n", currentLevel.sectorCount, MAX_SECTORS_ON_LEVEL, totalWalls, name);
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