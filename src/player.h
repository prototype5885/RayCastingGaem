#ifndef PLAYER_H
#define PLAYER_H

#include "extra_math.h"

#include "structs.h"

class Player {
public:
  float moveDirRad = 0.0f;
  float speed = 0.0f;
  Vector2 pos{0.0f, 0.0f};
  float rotRad = 0.0f;
  float fov = 90.0f;

  Player(const float x, const float y, const float rotation) {
    pos.x = x;
    pos.y = y;
    rotRad = deg2rad(rotation);
  }
};

#endif