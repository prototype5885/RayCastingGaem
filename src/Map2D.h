#ifndef MAP2D_H
#define MAP2D_H

#include "Display.h"
#include "Player.h"

void DrawMap(DisplayData *dd, int8_t *map, Player *player);
void ToggleMap(bool *mapEnabled);

#endif