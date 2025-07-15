#include "colors.h"

#include <cstdint>
#include <format>
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
  auto [r, g, b] = SplitRGB(color);
  r = static_cast<uint8_t>(static_cast<float>(r) * multiplier);
  g = static_cast<uint8_t>(static_cast<float>(g) * multiplier);
  b = static_cast<uint8_t>(static_cast<float>(b) * multiplier);

  return MergeRGB({r, g, b});
}

uint16_t ColorDifference(const uint32_t color1, const uint32_t color2) {
  auto [r1, g1, b1] = SplitRGB(color1);
  auto [r2, g2, b2] = SplitRGB(color2);

  const uint16_t rDif = (r1 > r2) ? r1 - r2 : r2 - r1;
  const uint16_t gDiff = (g1 > g2) ? g1 - g2 : g2 - g1;
  const uint16_t bDiff = (b1 > b2) ? b1 - b2 : b2 - b1;

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
  auto [r, g, b] = SplitRGB(color);
  cout << format("r{}, g{}, b{}", r, g, b) << endl;
}
} // namespace color