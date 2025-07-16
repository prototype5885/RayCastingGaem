#define STB_IMAGE_IMPLEMENTATION

#include "stb/stb_image.h"

#include "texture.h"

#include "filesystem.h"
#include "utils.h"

#include <dirent.h>

#include <cstdint>
#include <iostream>
#include <map>
#include <set>
#include <string>

namespace texture {
std::map<std::string, Texture> textureList;

string GetTextureName(const uint8_t wallType) {
  if (wallType == 1) {
    return "wall1";
  }
  if (wallType == 2) {
    return "wall2";
  }
  return "missing";
}

bool CheckIfSupportedExtension(const char *ext) {
  if (ext == nullptr)
    return false;
  if (strcmp(ext, "jpg") == 0)
    return true;
  if (strcmp(ext, "jpeg") == 0)
    return true;
  if (strcmp(ext, "png") == 0)
    return true;
  if (strcmp(ext, "bmp") == 0)
    return true;
  if (strcmp(ext, "tga") == 0)
    return true;

  return false;
}

void LoadTextures(const set<uint8_t> &texturesToLoad) {
  using namespace std;
  namespace fs = filesystem;

  textureList.clear();

  // create a default texture as fallback
  uint32_t fallbackRgb = 0;
  fallbackRgb |= 0 << 24;
  fallbackRgb |= 255 << 16;
  fallbackRgb |= 0 << 8;
  fallbackRgb |= 255;

  // textureList["fallback"].colors.push_back(0x24);
  textureList["fallback"].colors.push_back(fallbackRgb);

  set<string> textureNames;

  for (const uint8_t value : texturesToLoad) {
    string textureName = GetTextureName(value);
    if (textureName != "missing") {
      textureNames.insert(GetTextureName(value));
    }
  }

  const char *folderPath = "assets/textures";
  DIR *dr = opendir(folderPath);
  if (dr) {
    dirent *en;
    while ((en = readdir(dr)) != nullptr) {
      const fs::File file = fs::SplitFileNameExtension(en->d_name);
      if (!CheckIfSupportedExtension(file.extension)) {
        continue;
      }
      if (!textureNames.count(file.name))
        continue;

      char filePath[MAX_FILEPATH_LENGTH];
      snprintf(filePath, MAX_FILEPATH_LENGTH, "%s/%s.%s", folderPath, file.name, file.extension);

      printf("Loading texture %s...\n", filePath);

      int width, height, n;
      uint8_t *data = stbi_load(filePath, &width, &height, &n, 0);
      if (data == nullptr) {
        cerr << "Failed to load: " << filePath << endl;
        exit(1);
      }

      textureList[file.name].width = width;
      textureList[file.name].height = height;

      for (int p = 0; p < width * height; p++) {
        const int x = p % width;
        const int y = p / width;

        const int index = (y * width + x) * n;

        uint32_t rgb = 0;
        rgb |= 0 << 24;
        rgb |= data[index + 0] << 16;
        rgb |= data[index + 1] << 8;
        rgb |= data[index + 2];

        // uint8_t vgaColor = ColorToVGA(rgb);
        // textureList[fileName].colors.push_back(vgaColor);
        textureList[file.name].colors.push_back(rgb);
      }
      printf("Loaded %s, bytes: %zu\n", filePath, textureList[file.name].colors.size());
      stbi_image_free(data);
    }
    closedir(dr);
  }
}
} // namespace texture