#ifndef PLAYER_H
#define PLAYER_H

#include "geometry.h"

namespace player {
constexpr float playerSpeedDefault = 4.0f;

extern float moveDirRad;
extern float speed;
extern geometry::Vector2 pos;
extern float z;
extern float rotRad;
extern float rotVerticalRad;
extern float fov;
extern float radius;

} // namespace player

#endif