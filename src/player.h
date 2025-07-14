#ifndef PLAYER_H
#define PLAYER_H

#include "structs.h"

namespace player {
constexpr float playerSpeedDefault = 4.0f;

extern float moveDirRad;
extern float speed;
extern Vector2 pos;
extern float rotRad;
extern float fov;

} // namespace player

#endif