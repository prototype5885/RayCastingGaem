#define STB_IMAGE_IMPLEMENTATION

#include "stb/stb_image.h"

#include "texture.h"

#include <cstdint>
#include <filesystem>
#include <format>
#include <iostream>
#include <set>
#include <string>
#include <vector>

using namespace std;
using namespace filesystem;

std::map<std::string, Texture> textureList;

string GetTextureName(uint8_t wallType) {
  if (wallType == 1) {
    return "wall1";
  } else if (wallType == 2) {
    return "wall2";
  }
  return "missing";
}

bool CheckIfSupportedExtension(string extension) {
  if (extension == ".jpg")
    return true;
  if (extension == ".jpeg")
    return true;
  if (extension == ".png")
    return true;
  if (extension == ".bmp")
    return true;
  if (extension == ".tga")
    return true;

  return false;
}

void LoadTextures(set<uint8_t> wallTypes) {
  // create a default texture as fallback
  uint32_t rgb = 0;
  rgb |= 0 << 24;
  rgb |= 255 << 16;
  rgb |= 0 << 8;
  rgb |= 255;

  textureList["fallback"].colors.push_back(rgb);

  set<string> textureNames;

  for (uint8_t value : wallTypes) {
    string textureName = GetTextureName(value);
    if (textureName != "missing") {
      textureNames.insert(GetTextureName(value));
    }
  }

  for (const directory_entry &file : directory_iterator("assets/textures")) {
    if (file.is_regular_file() && CheckIfSupportedExtension(file.path().extension().string())) {

      if (!textureNames.count(file.path().stem().string()))
        continue;

      string filePath = file.path().string();

      cout << format("Loading texture {}...\n", filePath);

      int width, height, n;
      uint8_t *data = stbi_load(filePath.c_str(), &width, &height, &n, 0);
      if (data == NULL) {
        cerr << "Failed to load: " << filePath << endl;
        exit(1);
      }

      string fileName = file.path().stem().string();
      textureList[fileName].width = width;
      textureList[fileName].height = height;

      for (int p = 0; p < width * height; p++) {
        const int x = p % width;
        const int y = p / width;

        int index = (y * width + x) * n;

        uint32_t rgb = 0;
        rgb |= 0 << 24;
        rgb |= data[index + 0] << 16;
        rgb |= data[index + 1] << 8;
        rgb |= data[index + 2];

        textureList[fileName].colors.push_back(rgb);
      }
      cout << format("Loaded {}, bytes: {}\n", filePath, textureList[fileName].colors.size());
      stbi_image_free(data);
    }
  }
}