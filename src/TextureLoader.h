#ifndef TEXTURELOADER_H
#define TEXTURELOADER_H

#include <cstdint>
#include <string>

using namespace std;

uint32_t *LoadTexture(string name, int textureWidth, int textureHeight);

#endif