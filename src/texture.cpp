#define STB_IMAGE_IMPLEMENTATION

#include "stb/stb_image.h"

#include "texture.h"

#include <cstdint>
#include <filesystem>
#include <iostream>
#include <string>
#include <vector>

using namespace std;
using namespace filesystem;

std::map<std::string, Texture> textureList;

string GetTextureName(int wallType) {
  if (wallType == 1) {
    return "wall1.png";
  } else if (wallType == 2) {
    return "wall2.png";
  }
  return "";
}

void LoadTextures() {
  for (const directory_entry &file : directory_iterator("textures")) {
    if (file.is_regular_file() && file.path().extension() == ".png") {
      const char *filePath = file.path().string().c_str();

      int width, height, n;
      uint8_t *data = stbi_load(filePath, &width, &height, &n, 0);
      if (data == NULL) {
        cerr << "Failed to load: " << filePath << endl;
        exit(1);
      }

      string fileName = file.path().filename().string();
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
      stbi_image_free(data);
    }
  }
}