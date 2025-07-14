#include "colors.h"
#include "display.h"
#include "extra_math.h"
#include "level.h"
#include "player.h"
#include "structs.h"
#include "texture.h"

#include <cmath>
#include <cstdint>
#include <vector>

void CastRays(DisplayData *dd, Player const *player) {
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

    Texture *texture = NULL;

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

      if (currentLevel[i] != 0) {
        string textureName = GetTextureName(currentLevel[i]);
        try {
          texture = &textureList.at(textureName);
          break;
        } catch (const out_of_range &exception) {
          texture = &textureList.at("fallback");
          break;
        }
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

    // scales down texture if its wider or taller
    const float textureDimension = fminf(texture->width, texture->height);

    // calculates which pixels should be drawn vertically on each ray, like if wall segment is 400 pixel tall on the screen
    // and texture is 64 pixels tall, it will increment by 0,16 pixels from 0 to 6 drawing the nearest pixel to the value,
    // downwards direction
    const float pixelColumnOnEachRay = textureDimension / (float)wallHeight;

    // offset is needed for walls that are very close to the player so they wont stick to the top of the screen
    // it stays 0 if wall height is smaller than the screen height
    const float pixelColumnOffset = (wallHeight > dd->height) ? (wallHeight - dd->height) / 2.0f : 0;

    float horizontalSegment = pixelColumnOffset * pixelColumnOnEachRay;

    // this calculates on a scale from 0.0 to 1.0 which part of the wall the ray hit
    const float horizontalHitPoint = side ? hitPointX - floorf(hitPointX) : 1.0f - (hitPointY - floorf(hitPointY));

    for (int y = startPos; y < endPos; y++) {
      const int verticalSegment = (int)(textureDimension * horizontalHitPoint);

      int hpi = (int)horizontalSegment * texture->height + verticalSegment;
      hpi = clampi(hpi, 0, texture->width * texture->height - 1);

      horizontalSegment += pixelColumnOnEachRay;

      RGB rgb(texture->colors.at(hpi));
      rgb.Multiply(percentage);
      // dd->pixels[pixel * dd->width + ray] = rgb.ReturnRGB();
      AddPixelToBuffer(dd, ray, y, rgb.ReturnRGB());
    }
  }
}