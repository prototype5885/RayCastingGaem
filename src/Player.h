#ifndef PLAYER_H
#define PLAYER_H

#include "Structs.h"

#define PLAYER_SPEED_DEFAULT 4.0f;

typedef struct {
  float moveDirRad;
  float speed;
  Vector2 pos;
  float rotRad;
  float fov;
} Player;

Player NewPlayer(float x, float y, float rotation);

#endif