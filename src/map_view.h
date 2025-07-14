#ifndef MAPVIEW_H
#define MAPVIEW_H

#include "player.h"
#include "structs.h"

void DrawMap(const Player *player);
void ToggleMap(bool *mapEnabled);

#endif