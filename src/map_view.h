#ifndef MAPVIEW_H
#define MAPVIEW_H

namespace map_view {
extern bool mapEnabled;

void ZoomMap(int zoomValue);
void DrawMap();
void DrawRays(float angle, float distance);
void ToggleMap();
} // namespace map_view

#endif