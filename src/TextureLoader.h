#ifndef TEXTURELOADER_H
#define TEXTURELOADER_H

#include <cstdint>
#include <string>

uint32_t *LoadTexture(std::string name, int textureWidth, int textureHeight);

#endif