#include "structs.h"

#include <cstdint>

void AddPixelToBuffer(DisplayData *dd, int x, int y, uint32_t color) {
  int i = y * dd->width + x;
  if (i < dd->size && i > 0) {
    dd->pixels[i] = color;
  }
}