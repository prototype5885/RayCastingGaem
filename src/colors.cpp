#include "colors.h"

#include <cstdint>

RGB SplitRGB(uint32_t color) {
  RGB rgb;
  rgb.r = (color >> 16) & 0xFF;
  rgb.g = (color >> 8) & 0xFF;
  rgb.b = (color) & 0xFF;

  return rgb;
}

uint32_t MergeRGB(RGB rgb) {
  uint32_t color = 0;
  color |= 0 << 24;
  color |= rgb.r << 16;
  color |= rgb.g << 8;
  color |= rgb.b;

  return color;
}

uint32_t MultiplyRGB(uint32_t color, float multiplier) {
  RGB rgb = SplitRGB(color);
  rgb.r *= multiplier;
  rgb.g *= multiplier;
  rgb.b *= multiplier;

  return MergeRGB(rgb);
}