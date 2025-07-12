#ifndef COLORS_H
#define COLORS_H

#include "stdint.h"

#define WHITE_COLOR ((0 << 24) | (255 << 16) | (255 << 8) | 255)
#define GREY_COLOR ((0 << 24) | (50 << 16) | (50 << 8) | 50)
#define DARKER_GREY_COLOR ((0 << 24) | (30 << 16) | (30 << 8) | 30)
#define BLACK_COLOR ((0 << 24) | (0 << 16) | (0 << 8) | 0)
#define RED_COLOR ((0 << 24) | (255 << 16) | (0 << 8) | 0)
#define GREEN_COLOR ((0 << 24) | (0 << 16) | (255 << 8) | 0)
#define BLUE_COLOR ((0 << 24) | (0 << 16) | (0 << 8) | 255)
#define YELLOW_COLOR ((0 << 24) | (255 << 16) | (255 << 8) | 0)

typedef struct {
  uint8_t r;
  uint8_t g;
  uint8_t b;
} RGB;

uint32_t CreateRGB(uint8_t r, uint8_t g, uint8_t b);
RGB SplitRGB(uint32_t rgb);
uint32_t Multiply(uint32_t rgb, float multiplier);

#endif