#ifndef DISPLAY_H
#define DISPLAY_H

#include <cstdint>

namespace display {
extern float resScale;
extern int width, height, size;
extern uint32_t *pixels;

void AddPixelToBuffer(int x, int y, uint32_t color);
void AddPixelToBufferUnsafe(int x, int y, uint32_t color);
} // namespace display

#endif