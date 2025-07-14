#include "structs.h"

#include <cstdint>
#include <iostream>

using namespace std;

void AddPixelToBuffer(DisplayData *dd, int x, int y, uint32_t color) {
  int i = y * dd->width + x;
  if (0 <= i && i < dd->size) {
    dd->pixels[i] = color;
  } else {
    cerr << "Tried to put pixel out of display bounds at index: " << i << endl;
  }
}