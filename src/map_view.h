#ifndef MAPVIEW_H
#define MAPVIEW_H

#include "glm/vec2.hpp"

namespace map_view {
extern bool mapView;

void ZoomMap(int zoomValue);
void DrawMap();
void DrawRay(glm::vec2 to);
void ToggleMap();
} // namespace map_view

#endif