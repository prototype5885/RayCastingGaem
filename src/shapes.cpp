#include "display.h"
#include "extra_math.h"
#include "structs.h"

#include <cmath>
#include <cstdint>

void AddCircle(DisplayData *dd, float const radius, Vector2i const circlePos, uint32_t const color) {
  int x = static_cast<int>(radius);
  int y = 0;

  AddPixelToBuffer(dd, circlePos.x + x, circlePos.y + y, color);
  AddPixelToBuffer(dd, circlePos.x - x, circlePos.y + y, color);

  int p = 1 - radius;
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

void PlotLineLow(DisplayData *dd, Vector2i from, Vector2i to, uint32_t color) {
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
    AddPixelToBuffer(dd, x, y, color);
    if (d > 0) {
      y = y + yi;
      d = d + (2 * (dy - dx));
    } else {
      d = d + 2 * dy;
    }
  }
}

void PlotLineHigh(DisplayData *dd, Vector2i from, Vector2i to, uint32_t color) {
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
    AddPixelToBuffer(dd, x, y, color);
    if (d > 0) {
      x = x + xi;
      d = d + (2 * (dx - dy));
    } else {
      d = d + 2 * dx;
    }
  }
}

void AddLine(DisplayData *dd, Vector2i const from, Vector2i const to, uint32_t const color) {
  if (abs(to.y - from.y) < abs(to.x - from.x)) {
    if (from.x > to.x)
      PlotLineLow(dd, to, from, color);
    else
      PlotLineLow(dd, from, to, color);
  } else {
    if (from.y > to.y)
      PlotLineHigh(dd, to, from, color);
    else
      PlotLineHigh(dd, from, to, color);
  }

  AddPixelToBuffer(dd, from.x, from.y, color);
  AddPixelToBuffer(dd, to.x, to.y, color);
}

Vector2i CalculateLineEndpoint(Vector2i const from, float const length, float const angle) {
  Vector2i arrowEndPoint;
  arrowEndPoint.x = from.x + cosf(angle) * length;
  arrowEndPoint.y = from.y + sinf(angle) * length;
  return arrowEndPoint;
}

void AddLineWithArrow(DisplayData *dd, Vector2i const from, Vector2i const to, float const rot, uint32_t const color) {
  AddLine(dd, from, to, color);

  float arrowHeadAngle = deg2rad(135);
  for (int i = 0; i < 2; i++) {
    Vector2i arrowheadEndPoint = CalculateLineEndpoint(to, 6.0f, rot - arrowHeadAngle);
    AddLine(dd, to, arrowheadEndPoint, color);
    arrowHeadAngle += M_PI_2;
  }
}

void AddLineInDirectionWithArrow(DisplayData *dd, Vector2i const from, float const length, float const rot, uint32_t const color) {
  const Vector2i lineEndpoint = CalculateLineEndpoint(from, length, rot);
  AddLineWithArrow(dd, from, lineEndpoint, rot, color);
}

void AddLineInDirection(DisplayData *dd, Vector2i const from, float const length, float const rot, uint32_t const color) {
  const Vector2i lineEndpoint = CalculateLineEndpoint(from, length, rot);
  AddLine(dd, from, lineEndpoint, color);
}