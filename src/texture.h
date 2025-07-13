#ifndef TEXTURELOADER_H
#define TEXTURELOADER_H

#pragma once

#include <cstdint>
#include <map>
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

string GetTextureName(int wallType);
void LoadTextures();

#endif