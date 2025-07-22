#ifndef TEXTURE_H
#define TEXTURE_H

#include <cstdint>
#include <map>
#include <set>
#include <string>
#include <vector>

#define NO_TEXTURE "n"

namespace texture {
class Texture {
public:
  uint16_t width;
  uint16_t height;
  std::vector<uint32_t> colors;
};

extern Texture missingTexture;

extern std::map<std::string, Texture> textureList;

void LoadTextures(const std::set<std::string> &wallTextures);
} // namespace texture

#endif