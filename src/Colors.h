#ifndef COLORS_H
#define COLORS_H

#include <cstdint>

#define WHITE_COLOR ((0 << 24) | (255 << 16) | (255 << 8) | 255)
#define GREY_COLOR ((0 << 24) | (50 << 16) | (50 << 8) | 50)
#define DARKER_GREY_COLOR ((0 << 24) | (30 << 16) | (30 << 8) | 30)
#define BLACK_COLOR ((0 << 24) | (0 << 16) | (0 << 8) | 0)
#define RED_COLOR ((0 << 24) | (255 << 16) | (0 << 8) | 0)
#define GREEN_COLOR ((0 << 24) | (0 << 16) | (255 << 8) | 0)
#define BLUE_COLOR ((0 << 24) | (0 << 16) | (0 << 8) | 255)
#define YELLOW_COLOR ((0 << 24) | (255 << 16) | (255 << 8) | 0)

class RGB {
private:
  uint8_t r, g, b;

public:
  RGB(uint32_t rgb) {
    r = (rgb >> 16) & 0xFF;
    g = (rgb >> 8) & 0xFF;
    b = (rgb) & 0xFF;
  }

  uint32_t ReturnRGB() {
    uint32_t rgb = 0;
    rgb |= 0 << 24;
    rgb |= r << 16;
    rgb |= g << 8;
    rgb |= b;

    return rgb;
  }

  void Multiply(float multiplier) {
    r *= multiplier;
    g *= multiplier;
    b *= multiplier;
  }
};

#endif