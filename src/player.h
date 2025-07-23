#ifndef PLAYER_H
#define PLAYER_H

#include <vec2.hpp>

namespace player {
constexpr float playerSpeedDefault = 4.0f;

extern float moveDirRad;
extern float speed;
extern glm::vec2 pos;
extern float z;
extern float rotRad;
extern glm::vec2 forwardVector;
extern float rotVerticalRad;
extern float fov;
extern float radius;

} // namespace player

#endif