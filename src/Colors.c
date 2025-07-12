#include "Colors.h"
#include "stdint.h"

uint32_t CreateRGB(uint8_t r, uint8_t g, uint8_t b) {
  uint32_t rgb = 0;
  rgb |= 0 << 24;
  rgb |= r << 16;
  rgb |= g << 8;
  rgb |= b;

  return rgb;
}

RGB SplitRGB(uint32_t color) {
  RGB rgb;
  rgb.r = (color >> 16) & 0xFF;
  rgb.g = (color >> 8) & 0xFF;
  rgb.b = (color) & 0xFF;

  return rgb;
}

uint32_t Multiply(uint32_t color, float multiplier) {
  RGB rgb = SplitRGB(color);

  rgb.r *= multiplier;
  rgb.g *= multiplier;
  rgb.b *= multiplier;

  return CreateRGB(rgb.r, rgb.g, rgb.b);
}