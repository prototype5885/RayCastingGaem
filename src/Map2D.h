#ifndef MAP2D_H
#define MAP2D_H

#include "Player.h"
#include "Structs.h"
#include <cstdint>

void DrawMap(DisplayData *dd, int8_t *map, Player *player);
void ToggleMap(bool *mapEnabled);

#endif