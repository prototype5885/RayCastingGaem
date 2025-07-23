#ifndef SHAPES_H
#define SHAPES_H

#include <cstdint>

void AddCircle(float radius, glm::ivec2 circlePos, uint32_t color);
void PlotLineLow(glm::ivec2 from, glm::ivec2 to, uint32_t color);
void PlotLineHigh(glm::ivec2 from, glm::ivec2 to, uint32_t color);
void AddLine(glm::ivec2 from, glm::ivec2 to, uint32_t color);
glm::ivec2 CalculateLineEndpoint(glm::ivec2 from, float length, float angle);
void AddLineWithArrow(glm::ivec2 from, glm::ivec2 to, float rot, uint32_t color);
void AddLineInDirectionWithArrow(glm::ivec2 from, float length, float rot, uint32_t color);
void AddLineInDirection(glm::ivec2 from, float length, float rot, uint32_t color);

#endif