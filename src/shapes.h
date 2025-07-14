#ifndef SHAPES_H
#define SHAPES_H

#include "structs.h"

#include <cstdint>

void AddCircle(float radius, Vector2i circlePos, uint8_t color);
void PlotLineLow(Vector2i from, Vector2i to, uint8_t color);
void PlotLineHigh(Vector2i from, Vector2i to, uint8_t color);
void AddLine(Vector2i from, Vector2i to, uint8_t color);
Vector2i CalculateLineEndpoint(Vector2i from, float length, float angle);
void AddLineWithArrow(Vector2i from, Vector2i to, float rot, uint8_t color);
void AddLineInDirectionWithArrow(Vector2i from, float length, float rot, uint8_t color);
void AddLineInDirection(Vector2i from, float length, float rot, uint8_t color);

#endif