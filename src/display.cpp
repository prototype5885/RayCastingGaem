#include "display.h"

#include <cstdint>

namespace display {
float resScale = 1.0f;
int width = 1920;
int height = 1080;
int size = width * height;
uint32_t *pixels;
} // namespace display
