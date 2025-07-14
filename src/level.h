#ifndef LEVEL_H
#define LEVEL_H

#include <cstdint>
#include <string>
#include <vector>

using namespace std;

extern vector<uint8_t> currentLevel;

int LoadLevel(string name);

#endif