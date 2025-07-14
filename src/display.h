#ifndef DISPLAY_H
#define DISPLAY_H

#include "structs.h"

#include <cstdint>

void AddPixelToBuffer(const DisplayData *dd, int x, int y, uint32_t color);

#endif