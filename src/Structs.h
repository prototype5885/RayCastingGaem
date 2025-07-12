#ifndef STRUCTS_H
#define STRUCTS_H

#include "stdint.h"

typedef struct {
  int width, height, size;
  uint32_t *pixels;
} DisplayData;

typedef struct {
  float x, y;
} Vector2;

typedef struct {
  int x, y;
} Vector2i;

typedef struct {
  int8_t x, y;
} Vector2i8;

#endif