#ifndef MAPVIEW_H
#define MAPVIEW_H

namespace map_view {
extern bool mapEnabled;

void DrawMap();
void ToggleMap(bool *mapEnabled);
} // namespace map_view

#endif