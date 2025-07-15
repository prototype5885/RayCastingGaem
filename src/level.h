#ifndef LEVEL_H
#define LEVEL_H

#include "geometry.h"

#include <cstdint>
#include <string>
#include <vector>

using namespace std;

namespace level {
const vector<geometry::Line2D> walls = {{5, 5, 50, 5},    {50, 5, 50, 50},  {50, 50, 5, 50},  {5, 50, 5, 5},    {20, 10, 20, 40}, {30, 10, 40, 20},
                                        {30, 40, 40, 30}, {25, 20, 35, 20}, {35, 30, 45, 30}, {45, 30, 45, 45}, {45, 45, 35, 45}};
extern vector<uint8_t> currentLevel;
int LoadLevel(string name);
geometry::Vector2 GetMapDimension();
} // namespace level

#endif