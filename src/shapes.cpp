#include "display.h"
#include "glm/trigonometric.hpp"
#include "glm/vec2.hpp"

#include <cmath>
#include <cstdint>

using display::AddPixelToBuffer;
using glm::ivec2;

void AddCircle(float const radius, ivec2 const circlePos, uint32_t const color) {
  int x = static_cast<int>(radius);
  int y = 0;

  AddPixelToBuffer(circlePos.x + x, circlePos.y + y, color);
  AddPixelToBuffer(circlePos.x - x, circlePos.y + y, color);

  int p = static_cast<int>(1.0f - radius);
  while (x > y) {
    y++;

    if (p <= 0) {
      p = p + 2 * y + 1;
    } else {
      x--;
      p = p + 2 * y - 2 * x + 1;
    }

    if (x < y)
      break;
  }
}

void PlotLineLow(const ivec2 from, const ivec2 to, const uint32_t color) {
  const int dx = to.x - from.x;
  int dy = to.y - from.y;

  int yi = 1;

  if (dy < 0) {
    yi = -1;
    dy = -dy;
  }

  int d = 2 * dy - dx;
  int y = from.y;

  for (int x = from.x; x < to.x; x++) {
    AddPixelToBuffer(x, y, color);
    if (d > 0) {
      y = y + yi;
      d = d + 2 * (dy - dx);
    } else {
      d = d + 2 * dy;
    }
  }
}

void PlotLineHigh(const ivec2 from, const ivec2 to, const uint32_t color) {
  int dx = to.x - from.x;
  const int dy = to.y - from.y;

  int xi = 1;

  if (dx < 0) {
    xi = -1;
    dx = -dx;
  }

  int d = 2 * dx - dy;
  int x = from.x;

  for (int y = from.y; y < to.y; y++) {
    AddPixelToBuffer(x, y, color);
    if (d > 0) {
      x = x + xi;
      d = d + 2 * (dx - dy);
    } else {
      d = d + 2 * dx;
    }
  }
}

void AddLine(ivec2 const from, ivec2 const to, uint32_t const color) {
  if (abs(to.y - from.y) < abs(to.x - from.x)) {
    if (from.x > to.x)
      PlotLineLow(to, from, color);
    else
      PlotLineLow(from, to, color);
  } else {
    if (from.y > to.y)
      PlotLineHigh(to, from, color);
    else
      PlotLineHigh(from, to, color);
  }

  AddPixelToBuffer(from.x, from.y, color);
  AddPixelToBuffer(to.x, to.y, color);
}

ivec2 CalculateLineEndpoint(ivec2 const from, float const length, float const angle) {
  int x = static_cast<int>(static_cast<float>(from.x) + cosf(angle) * length);
  int y = static_cast<int>(static_cast<float>(from.y) + sinf(angle) * length);
  return {x, y};
}

void AddLineWithArrow(ivec2 const from, ivec2 const to, float const rot, uint32_t const color) {
  AddLine(from, to, color);

  float arrowHeadAngle = glm::radians(135.0f);
  for (int i = 0; i < 2; i++) {
    const ivec2 arrowheadEndPoint = CalculateLineEndpoint(to, 6.0f, rot - arrowHeadAngle);
    AddLine(to, arrowheadEndPoint, color);
    arrowHeadAngle += M_PI_2;
  }
}

void AddLineInDirectionWithArrow(ivec2 const from, float const length, float const rot, uint32_t const color) {
  const ivec2 lineEndpoint = CalculateLineEndpoint(from, length, rot);
  AddLineWithArrow(from, lineEndpoint, rot, color);
}

void AddLineInDirection(ivec2 const from, float const length, float const rot, uint32_t const color) {
  const ivec2 lineEndpoint = CalculateLineEndpoint(from, length, rot);
  AddLine(from, lineEndpoint, color);
}