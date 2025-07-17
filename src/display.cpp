#include "display.h"

#include <cstdint>
#include <cstdio>

namespace display {
float resScale = 1.0f;
int width = 1920;
int height = 1080;
int size = width * height;
uint32_t *pixels;
} // namespace display

void display::AddPixelToBuffer(const int x, const int y, const uint32_t color) {
  const int i = y * width + x;
  if (0 <= x && x < width && 0 <= y && y < height) {
    pixels[i] = color;
  } else {
    // printf("Tried to put pixel out of display bounds at index: %d\n", i);
  }
}

void display::AddPixelToBufferUnsafe(const int x, const int y, const uint32_t color) {
  // display::pixels[y * display::width + x] = vga_palette[color];
  pixels[y * width + x] = color;
}
