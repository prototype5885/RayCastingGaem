#ifndef TEXTURE_H
#define TEXTURE_H

#include <cstdint>
#include <map>
#include <set>
#include <string>
#include <vector>

#define NO_TEXTURE "n"

using namespace std;

namespace texture {
struct Texture {
  uint16_t width;
  uint16_t height;
  vector<uint32_t> colors;
};

extern Texture missingTexture;

extern std::map<std::string, Texture> textureList;

void LoadTextures(const set<string> &wallTextures);
} // namespace texture

#endif