#include "colors.h"
#include "extra_math.h"
#include "player.h"
#include "structs.h"
#include "texture.h"

#include <cmath>
#include <cstdint>

void CastRays(DisplayData const *dd, Player const *player, int8_t const *map) {
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

    Texture *texture;

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

      if (map[i] != 0) {
        string textureName = GetTextureName(map[i]);
        try {
          texture = &textureList.at(textureName);
          break;
        } catch (const out_of_range &exception) {
          cerr << "Texture '" << textureName << "' was not found, exception type: '" << exception.what() << "'" << endl;
          exit(1);
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

    const float stepBetweenHorizontalSegments = (float)texture->height / (float)wallHeight;

    // offset is needed for walls that are very close to the player so they wont stick to the top of the screen
    // it stays 0 if wall height is smaller than the screen height
    const float offset = (wallHeight > dd->height) ? (wallHeight - dd->height) / 2.0f : 0;

    float horizontalSegment = offset * stepBetweenHorizontalSegments;

    const float c = side ? hitPointX - floorf(hitPointX) : hitPointY - floorf(hitPointY);

    for (int pixel = startPos; pixel < endPos; pixel++) {
      const int verticalSegment = texture->height * c;

      const int hpi = (int)horizontalSegment * texture->height + verticalSegment; // horizontal pixel index
      horizontalSegment += stepBetweenHorizontalSegments;
      RGB rgb(texture->colors[hpi]);
      rgb.Multiply(percentage);
      dd->pixels[pixel * dd->width + ray] = rgb.ReturnRGB();
    }
  }
}