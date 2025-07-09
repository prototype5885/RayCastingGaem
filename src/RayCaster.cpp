#include "Colors.h"
#include "ExtraMath.h"
#include "Player.h"
#include "Structs.h"
#include <cmath>

void CastRays(DisplayData const *dd, Player const *player, int8_t const *map, uint32_t const *tileMap) {
  for (int ray = 0; ray < dd->width; ray++) {
    const float aspectRatio = static_cast<float>(dd->width) / static_cast<float>(dd->height);
    float rayAngle = player->rotRad - (aspectRatio / 2.0f);        // start angle of leftmost ray relative to player rotation
    rayAngle += (deg2rad(ray) / deg2rad(dd->width)) * aspectRatio; // then increment each ray in radian by this amount to the right

    const float dx = cosf(rayAngle);
    const float dy = sinf(rayAngle);

    int mapX = player->pos.x;
    int mapY = player->pos.y;

    float sideDistX, sideDistY;

    const float deltaDistX = fabsf(1.0f / dx);
    const float deltaDistY = fabsf(1.0f / dy);

    float distance;

    int stepX, stepY;

    float hitPointX, hitPointY;

    int tOffset = 0; // texture offset

    if (dx < 0.0f) {
      stepX = -1;
      sideDistX = (player->pos.x - (float)mapX) * deltaDistX;
    } else {
      stepX = 1;
      sideDistX = ((float)mapX + 1.0f - player->pos.x) * deltaDistX;
    }
    if (dy < 0.0f) {
      stepY = -1;
      sideDistY = (player->pos.y - (float)mapY) * deltaDistY;
    } else {
      stepY = 1;
      sideDistY = ((float)mapY + 1.0f - player->pos.y) * deltaDistY;
    }

    bool side = false;
    int attempt = 0;
    while (attempt < 64) {
      attempt++;

      if (sideDistX < sideDistY) {
        sideDistX += deltaDistX;
        mapX += stepX;
        side = false;
      } else {
        sideDistY += deltaDistY;
        mapY += stepY;
        side = true;
      }

      const int i = mapY * 16 + mapX;

      if (i > 255) {
        return;
      }

      int wallType = map[i];
      if (wallType == 1) {
        tOffset = 0 * 4096;
        break;
      } else if (wallType == 2) {
        tOffset = 1 * 4096;
        break;
      }
    }

    if (!side) { // if hit a horizontal wall
      distance = ((float)mapX - player->pos.x + (1.0f - (float)stepX) / 2.0f) / dx;
      hitPointX = (float)mapX + ((float)stepX / 2.0f);
      hitPointY = player->pos.y + distance * dy;
    } else { // if hit a vertical wall
      distance = (mapY - player->pos.y + (1.0f - stepY) / 2.0f) / dy;
      hitPointX = player->pos.x + distance * dx;
      hitPointY = mapY + ((float)stepY / 2.0f);
    }

    distance = distance * cosf(rayAngle - player->rotRad); // fisheye fix

    // const int wallHeight = height / distance * (100.0f / player.fov); // this is how tall the wall will be based on ray distance
    const int wallHeight = dd->height / distance; // this is how tall the wall will be based on ray distance
    const int middle = dd->height / 2;            // middle of the screen

    int startPos = middle - wallHeight / 2; // wall starts at this height
    if (startPos < 0)                       // prevent it from starting from above the screen
      startPos = 0;

    int endPos = middle + wallHeight / 2; // wall ends here
    if (endPos > dd->height)              // prevent it from starting from below the screen
      endPos = dd->height;

    float percentage = 1.0 - (distance - 4.0f) / (16.0f - 4.0f);

    if (distance < 4.0f) {
      percentage = 1.0f;
    } else if (distance > 16.0f) {
      percentage = 0.0f;
    }

    if (percentage < 0.25f)
      percentage = 0.25f;

    const float stepBetweenHorizontalSegments = 64.0f / (float)wallHeight;

    // offset is needed for walls that are very close to the player so they wont stick to the top of the screen
    // it stays 0 if wall height is smaller than the screen height
    const float offset = (wallHeight > dd->height) ? (wallHeight - dd->height) / 2.0f : 0;

    float horizontalSegment = offset * stepBetweenHorizontalSegments;

    const float c = side ? hitPointX - floorf(hitPointX) : hitPointY - floorf(hitPointY);

    for (int pixel = startPos; pixel < endPos; pixel++) {
      const int verticalSegment = 64 * c;

      const int hpi = (int)horizontalSegment * 64 + verticalSegment; // horizontal pixel index
      horizontalSegment += stepBetweenHorizontalSegments;
      RGB rgb;
      rgb.CreateRGB(tileMap[hpi + tOffset]);
      rgb.Multiply(percentage);
      const uint32_t reColor = rgb.ReturnRGB();

      dd->pixels[pixel * dd->width + ray] = reColor;
    }
  }
}