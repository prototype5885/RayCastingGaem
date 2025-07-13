#ifndef RAYCASTER_H
#define RAYCASTER_H

#include "player.h"
#include "structs.h"

#include <cstdint>
#include <vector>

void CastRays(DisplayData const *dd, Player const *player, vector<uint8_t> map);

#endif