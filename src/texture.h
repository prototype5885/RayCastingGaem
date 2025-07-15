#ifndef TEXTURELOADER_H
#define TEXTURELOADER_H

#pragma once

#include <cstdint>
#include <map>
#include <set>
#include <string>
#include <vector>

using namespace std;

namespace texture {
class Texture {
public:
  uint16_t width;
  uint16_t height;
  vector<uint32_t> colors;
};

void LoadTextures(const set<uint8_t> &texturesToLoad);
} // namespace texture

#endif