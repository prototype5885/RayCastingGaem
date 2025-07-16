#ifndef TEXTURE_H
#define TEXTURE_H

#include <cstdint>
#include <set>
#include <vector>

using namespace std;

namespace texture {
struct Texture {
  uint16_t width;
  uint16_t height;
  vector<uint32_t> colors;
};

void LoadTextures(const set<uint8_t> &texturesToLoad);
} // namespace texture

#endif