#ifndef TEXTURE_H
#define TEXTURE_H

#include <cstdint>
#include <map>
#include <set>
#include <string>
#include <vector>

using namespace std;

namespace texture {
struct Texture {
  uint16_t width;
  uint16_t height;
  vector<uint32_t> colors;
};

extern std::map<std::string, Texture> textureList;

void LoadTextures(const set<uint8_t> &texturesToLoad);
} // namespace texture

#endif