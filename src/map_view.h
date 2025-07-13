#ifndef MAPVIEW_H
#define MAPVIEW_H

#include "player.h"
#include "structs.h"

#include <cstdint>
#include <vector>

void DrawMap(DisplayData *dd, Player *player);
void ToggleMap(bool *mapEnabled);

#endif