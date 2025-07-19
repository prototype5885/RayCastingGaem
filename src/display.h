#ifndef DISPLAY_H
#define DISPLAY_H

#include "colors.h"

#include <cstdint>

namespace display {
extern float resScale;
extern int width, height, size;
extern uint32_t *pixels;

inline void DrawPixel(const int i, const uint32_t color) {
  const uint8_t alpha = color::GetAlpha(color);
  if (alpha == UINT8_MAX) {
    pixels[i] = color;
  } else if (alpha != 0) {
    pixels[i] = color::ColorBlending(pixels[i], color);
  }
}

inline void AddPixelToBuffer(const int x, const int y, const uint32_t color) {
  if (0 <= x && x < width && 0 <= y && y < height) {
    DrawPixel(y * width + x, color);
  } else {
    // printf("Tried to put pixel out of display bounds at index: %d\n", i);
  }
}

inline void AddPixelToBufferUnsafe(const int x, const int y, const uint32_t color) { DrawPixel(y * width + x, color); }

} // namespace display

#endif