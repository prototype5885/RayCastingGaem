#include "colors.h"

#include <cstdint>
#include <iostream>

using namespace std;

namespace color {
RGB SplitRGB(const uint32_t color) {
  RGB rgb;
  rgb.r = color >> 16 & 0xFF;
  rgb.g = color >> 8 & 0xFF;
  rgb.b = color & 0xFF;

  return rgb;
}

uint32_t MergeRGB(const RGB rgb) {
  uint32_t color = 0;
  color |= 0 << 24;
  color |= rgb.r << 16;
  color |= rgb.g << 8;
  color |= rgb.b;

  return color;
}

uint32_t MultiplyRGB(const uint32_t color, const float multiplier) {
  const RGB c = SplitRGB(color);
  const uint8_t r = static_cast<uint8_t>(static_cast<float>(c.r) * multiplier);
  const uint8_t g = static_cast<uint8_t>(static_cast<float>(c.g) * multiplier);
  const uint8_t b = static_cast<uint8_t>(static_cast<float>(c.b) * multiplier);

  return MergeRGB({r, g, b});
}

uint16_t ColorDifference(const uint32_t color1, const uint32_t color2) {
  const RGB c1 = SplitRGB(color1);
  const RGB c2 = SplitRGB(color2);

  const uint16_t rDif = c1.r > c2.r ? c1.r - c2.r : c2.r - c1.r;
  const uint16_t gDiff = c1.g > c2.g ? c1.g - c2.g : c2.g - c1.g;
  const uint16_t bDiff = c1.b > c2.b ? c1.b - c2.b : c2.b - c1.b;

  return rDif + gDiff + bDiff;
}

// uint8_t ColorToVGA(const uint32_t color) {
//   uint8_t nearest = 0;
//   uint16_t lastDiff = UINT16_MAX;
//   for (int i = 0; i < VGA_PALETTE_LENGTH; i++) {
//     const uint16_t diff = ColorDifference(color, vga_palette[i]);
//     if (diff < lastDiff) {
//       nearest = static_cast<uint8_t>(i);
//       lastDiff = diff;
//     }
//   }
//
//   return nearest;
// }

void PrintRGB(const uint32_t color) {
  const RGB rgb = SplitRGB(color);
  printf("r%u, g%u, b%u\n", rgb.r, rgb.g, rgb.b);
}
} // namespace color