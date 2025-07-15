#ifndef MAPVIEW_H
#define MAPVIEW_H

#include <cstdint>

namespace map_view {
extern uint8_t mapViewMode;

void ZoomMap(int zoomValue);
void DrawMap();
void DrawRays(float angle, float distance);
void ToggleMapMode();
} // namespace map_view

#endif