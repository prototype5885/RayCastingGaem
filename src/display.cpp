#include "colors.h"

#include <cstdint>
#include <iostream>

using namespace std;

namespace display {
int width, height, size;
uint32_t *pixels;
} // namespace display

void AddPixelToBuffer(const int x, const int y, const uint8_t color) {
  int i = y * display::width + x;
  if (0 <= i && i < display::size) {
    display::pixels[i] = vga_palette[color];
  } else {
    cerr << "Tried to put pixel out of display bounds at index: " << i << endl;
  }
}

void AddPixelToBufferUnsafe(const int x, const int y, const uint8_t color) { display::pixels[y * display::width + x] = vga_palette[color]; }