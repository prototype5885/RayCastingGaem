#include "level.h"
#include "texture.h"

#include <fstream>
#include <geometric.hpp>
#include <iostream>
#include <sstream>

namespace level {
Level currentLevel{};

void LoadLevel(const std::string &name) {
  using namespace std;
  cout << "Loading level " << name << endl;

  string filePath = "assets/levels/" + name + ".txt";

  ifstream file(filePath);

  if (!file.is_open()) {
    throw runtime_error("Could not open file " + filePath);
  }

  currentLevel.sectors.clear();
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
      Sector sector;
      Wall wall{};

      // sets the starting point for the first wall
      if (iss >> wall.from.x >> wall.from.y >> sector.bottom >> sector.top) {
      } else {
        throw runtime_error("Failed parsing level file " + filePath + ", error at line " + to_string(lineCounter));
      }

      sector.walls.push_back(wall);
      currentLevel.sectors.push_back(sector);

    } else if (typeIdentifier == 'w') { // if wall
      Sector &sector = currentLevel.sectors.back();
      Wall &wall = sector.walls.back();

      // don't run this if first wall because starting point has been already set in first 's'
      if (sector.walls.size() > 1) {
        // grab the end point from the previous wall to use as the new wall's starting point
        wall.from.x = sector.walls.at(sector.walls.size() - 1).to.x;
        wall.from.y = sector.walls.at(sector.walls.size() - 1).to.y;
      }

      // sets the target point for the current wall
      if (iss >> wall.to.x >> wall.to.y >> wall.textureBottom >> wall.textureMid >> wall.textureTop >> wall.collision) {
      } else {
        throw runtime_error("Failed parsing level file " + filePath + ", error at line " + to_string(lineCounter));
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

      wall.wallLength = glm::distance(glm::vec2{wall.from.x, wall.from.y}, glm::vec2{wall.to.x, wall.to.y});

      wallTextures.insert(wall.textureBottom);
      wallTextures.insert(wall.textureMid);
      wallTextures.insert(wall.textureTop);

      sector.walls.push_back(wall);

      totalWalls++;

    } else if (typeIdentifier == 'e') { // if sector end
      // check if the first sector is enclosed, first and last point must match
      const vector<Wall> walls = currentLevel.sectors.back().walls;
      if (walls.front().from != walls.back().to) {
        char errorMessage[128];
        snprintf(errorMessage, sizeof(errorMessage), "Sector is not enclosed, starts at (%f, %f), ends at (%f, %f)\n", walls.front().from.x,
                 walls.front().from.y, walls.back().to.x, walls.back().to.y);
        throw runtime_error(errorMessage);
      }
    }
    lineCounter++;
  }
  currentLevel.name = name;
  texture::LoadTextures(wallTextures);
  std::cout << "Successfully loaded " << currentLevel.sectors.size() << " sectors with " << totalWalls << " walls from " << name << std::endl;
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