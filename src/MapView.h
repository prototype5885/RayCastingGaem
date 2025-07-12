#ifndef MAPVIEW_H
#define MAPVIEW_H

#include "Player.h"
#include "Structs.h"
#include "stdint.h"

void DrawMap(DisplayData *dd, int8_t *map, Player *player);
void ToggleMap(char *mapEnabled);

#endif