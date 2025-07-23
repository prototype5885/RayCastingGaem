#define STB_IMAGE_IMPLEMENTATION

#include "texture.h"
#include "colors.h"
#include "stb_image.h"

#include <filesystem>
#include <iostream>
#include <map>
#include <set>
#include <string>

namespace texture {
std::map<std::string, Texture> textureList;

Texture missingTexture = {1, 1, {color::MergeRGB({255, 255, 0, 255})}};

bool CheckIfSupportedExtension(const std::string &ext) {
  if (ext.empty())
    return false;
  if (ext == ".jpg")
    return true;
  if (ext == ".jpeg")
    return true;
  if (ext == ".png")
    return true;
  if (ext == ".bmp")
    return true;
  if (ext == ".tga")
    return true;

  return false;
}

void LoadTextures(const std::set<std::string> &wallTextures) {
  using namespace std;
  namespace fs = filesystem;

  textureList.clear();

  const fs::path folderPath = "assets/textures";
  for (const auto &file : fs::directory_iterator(folderPath)) {
    const string path = file.path().string();
    const string name = file.path().stem().string();
    const string extension = file.path().extension().string();

    // check if texture needs to be loaded
    if (!wallTextures.count(name))
      continue;

    // check if it's supported picture format
    if (!CheckIfSupportedExtension(extension))
      continue;

    cout << "Loading texture " << path << "..." << endl;

    int width, height, n;
    uint8_t *data = stbi_load(path.c_str(), &width, &height, &n, 0);
    if (data == nullptr) {
      throw runtime_error("Failed to load texture " + file.path().string());
    }

    textureList[name].width = width;
    textureList[name].height = height;

    for (int p = 0; p < width * height; p++) {
      const int x = p % width;
      const int y = p / width;

      const int index = (y * width + x) * n;

      const uint8_t a = n == 4 ? data[index + 3] : 255;
      const uint8_t r = data[index + 0];
      const uint8_t g = data[index + 1];
      const uint8_t b = data[index + 2];

      const uint32_t color = color::MergeRGB({a, r, g, b});

      // uint8_t vgaColor = ColorToVGA(rgb);
      // textureList[fileName].colors.push_back(vgaColor);
      textureList[name].colors.push_back(color);
    }
    cout << "Loaded " << path << ", bytes:" << textureList[name].colors.size() << endl;
    stbi_image_free(data);
  }

  // this will check if a texture file wasn't loaded and then replace it with the fallback missing texture
  for (auto name = wallTextures.begin(); name != wallTextures.end(); ++name) {
    if (textureList.find(*name) == textureList.end()) {
      cout << "Couldn't load texture " << *name << endl;
      textureList[*name] = missingTexture;
    }
  }
}
} // namespace texture