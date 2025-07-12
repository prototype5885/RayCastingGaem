#ifndef RAYCASTER_H
#define RAYCASTER_H

#include "Player.h"
#include "Structs.h"
#include "stdint.h"

void CastRays(DisplayData const *dd, Player const *player, int8_t const *map, uint32_t const *tileMap);

#endif