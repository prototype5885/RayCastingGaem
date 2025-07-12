#ifndef RAYCASTER_H
#define RAYCASTER_H

#include "player.h"
#include "structs.h"

#include <cstdint>

void CastRays(DisplayData const *dd, Player const *player, int8_t const *map, uint32_t const *tileMap);

#endif