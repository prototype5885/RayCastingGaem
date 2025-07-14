#include "display.h"
#include "extra_math.h"

#include <cmath>
#include <cstdint>

void AddCircle(float const radius, Vector2i const circlePos, uint8_t const color) {
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

void PlotLineLow(const Vector2i from, const Vector2i to, const uint8_t color) {
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

void PlotLineHigh(const Vector2i from, const Vector2i to, const uint8_t color) {
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

void AddLine(Vector2i const from, Vector2i const to, uint8_t const color) {
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

Vector2i CalculateLineEndpoint(Vector2i const from, float const length, float const angle) {
  Vector2i arrowEndPoint;
  arrowEndPoint.x = static_cast<int>(static_cast<float>(from.x) + cosf(angle) * length);
  arrowEndPoint.y = static_cast<int>(static_cast<float>(from.y) + sinf(angle) * length);
  return arrowEndPoint;
}

void AddLineWithArrow(Vector2i const from, Vector2i const to, float const rot, uint8_t const color) {
  AddLine(from, to, color);

  float arrowHeadAngle = deg2rad(135);
  for (int i = 0; i < 2; i++) {
    const Vector2i arrowheadEndPoint = CalculateLineEndpoint(to, 6.0f, rot - arrowHeadAngle);
    AddLine(to, arrowheadEndPoint, color);
    arrowHeadAngle += M_PI_2;
  }
}

void AddLineInDirectionWithArrow(Vector2i const from, float const length, float const rot, uint8_t const color) {
  const Vector2i lineEndpoint = CalculateLineEndpoint(from, length, rot);
  AddLineWithArrow(from, lineEndpoint, rot, color);
}

void AddLineInDirection(Vector2i const from, float const length, float const rot, uint8_t const color) {
  const Vector2i lineEndpoint = CalculateLineEndpoint(from, length, rot);
  AddLine(from, lineEndpoint, color);
}