#ifndef DISPLAY_H
#define DISPLAY_H

#include <cstdint>
#include "Structs.h"

void AddPixelToBuffer(DisplayData *dd, int x, int y, uint32_t color);

#endif