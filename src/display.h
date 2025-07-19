#ifndef DISPLAY_H
#define DISPLAY_H

#include "colors.h"

#include <cstdint>

namespace display {
extern float resScale;
extern int width, height, size;
extern uint32_t *pixels;

inline void AddPixelToBuffer(const int x, const int y, const uint32_t color) {
  const int i = y * width + x;
  if (0 <= x && x < width && 0 <= y && y < height) {
    if (color::GetAlpha(color) == UINT8_MAX) {
      pixels[i] = color;
    }
  } else {
    // printf("Tried to put pixel out of display bounds at index: %d\n", i);
  }
};

inline void AddPixelToBufferUnsafe(const int x, const int y, const uint32_t color) {
  // display::pixels[y * display::width + x] = vga_palette[color];
  pixels[y * width + x] = color;
};
} // namespace display

#endif