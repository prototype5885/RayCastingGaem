#ifndef MAPVIEW_H
#define MAPVIEW_H

#include "player.h"
#include "structs.h"

#include <cstdint>

void DrawMap(DisplayData *dd, int8_t *map, Player *player);
void ToggleMap(bool *mapEnabled);

#endif