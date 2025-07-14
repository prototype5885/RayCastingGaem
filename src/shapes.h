#ifndef SHAPES_H
#define SHAPES_H

#include "structs.h"

#include <cstdint>

void AddCircle(DisplayData *dd, float radius, Vector2i circlePos, uint8_t color);
void PlotLineLow(const DisplayData *dd, Vector2i from, Vector2i to, uint8_t color);
void PlotLineHigh(const DisplayData *dd, Vector2i from, Vector2i to, uint8_t color);
void AddLine(const DisplayData *dd, Vector2i from, Vector2i to, uint8_t color);
Vector2i CalculateLineEndpoint(Vector2i from, float length, float angle);
void AddLineWithArrow(const DisplayData *dd, Vector2i from, Vector2i to, float rot, uint8_t color);
void AddLineInDirectionWithArrow(const DisplayData *dd, Vector2i from, float length, float rot, uint8_t color);
void AddLineInDirection(const DisplayData *dd, Vector2i from, float length, float rot, uint8_t color);

#endif