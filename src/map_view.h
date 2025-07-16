#ifndef MAPVIEW_H
#define MAPVIEW_H

#include "geometry.h"

namespace map_view {
extern bool mapView;

void ZoomMap(int zoomValue);
void DrawMap();
void DrawRay(geometry::Vector2 to);
void ToggleMap();
} // namespace map_view

#endif