#ifndef SHAPES_H
#define SHAPES_H

#include "Structs.h"
#include <cstdint>

void AddCircle(DisplayData *dd, float radius, Vector2i circlePos, uint32_t color);
void PlotLineLow(DisplayData *dd, Vector2i from, Vector2i to, uint32_t color);
void PlotLineHigh(DisplayData *dd, Vector2i from, Vector2i to, uint32_t color);
void AddLine(DisplayData *dd, Vector2i from, Vector2i to, uint32_t color);
Vector2i CalculateLineEndpoint(Vector2i from, float length, float angle);
void AddLineWithArrow(DisplayData *dd, Vector2i from, Vector2i to, float rot, uint32_t color);
void AddLineInDirectionWithArrow(DisplayData *dd, Vector2i from, float length, float rot, uint32_t color);
void AddLineInDirection(DisplayData *dd, Vector2i from, float length, float rot, uint32_t color);

#endif