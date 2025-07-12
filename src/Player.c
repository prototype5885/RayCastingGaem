#include "Player.h"
#include "ExtraMath.h"

Player NewPlayer(float x, float y, float rotation) {
  rotation = deg2rad(rotation);
  Player player = {0.0f, 0.0f, {x, y}, rotation, 90.0f};
  return player;
}