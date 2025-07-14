#ifndef TEXTURELOADER_H
#define TEXTURELOADER_H

#pragma once

#include <cstdint>
#include <map>
#include <set>
#include <string>
#include <vector>

using namespace std;

class Texture {
public:
  uint16_t width;
  uint16_t height;
  vector<uint32_t> colors;
};

extern map<string, Texture> textureList;

bool CheckIfSupportedExtension(const string &extension);
string GetTextureName(uint8_t wallType);
void LoadTextures(const set<uint8_t>& texturesToLoad);

#endif