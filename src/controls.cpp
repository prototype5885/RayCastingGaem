#include "controls.h"

#include "display.h"
#include "geometry.h"
#include "level.h"
#include "map_view.h"
#include "physics.h"
#include "player.h"
#include "utils.h"

#include <SDL2/SDL_events.h>
#include <algorithm>
#include <iostream>

namespace key {
int8_t W = 0;
int8_t S = 0;
int8_t A = 0;
int8_t D = 0;
} // namespace key

SDL_Event event;

void controls::HandleControls() {
  while (SDL_PollEvent(&event)) {
    switch (event.type) {
    case SDL_QUIT:
      utils::running = false;
      break;
    case SDL_KEYDOWN:
      if (event.key.keysym.sym == SDLK_ESCAPE) {
        utils::running = false;
      }
      if (event.key.keysym.sym == SDLK_w) {
        key::W = 1;
      }
      if (event.key.keysym.sym == SDLK_s) {
        key::S = 1;
      }
      if (event.key.keysym.sym == SDLK_a) {
        key::A = 1;
      }
      if (event.key.keysym.sym == SDLK_d) {
        key::D = 1;
      }
      if (event.key.keysym.sym == SDLK_TAB) {
        map_view::ToggleMapMode();
      }
      // if (event.key.keysym.sym == SDLK_n) {
      //   noiseEnabled = !noiseEnabled;
      // }
      break;
    case SDL_KEYUP:
      if (event.key.keysym.sym == SDLK_w) {
        key::W = 0;
      }
      if (event.key.keysym.sym == SDLK_s) {
        key::S = 0;
      }
      if (event.key.keysym.sym == SDLK_a) {
        key::A = 0;
      }
      if (event.key.keysym.sym == SDLK_d) {
        key::D = 0;
      }
      break;
    case SDL_MOUSEMOTION:
      player::rotRad += geometry::deg2rad(static_cast<float>(event.motion.xrel)) * display::resScale / 8;

      if (player::rotRad < -M_PI) {
        player::rotRad += 2 * M_PI;
      } else if (player::rotRad > M_PI) {
        player::rotRad -= 2 * M_PI;
      }
      break;
    case SDL_MOUSEWHEEL:
      if (map_view::mapViewMode != 0) {
        map_view::ZoomMap(event.wheel.y);
      }
    default:;
    }
  }

  const auto sideways = static_cast<int8_t>(key::D - key::A);
  const auto forwards = static_cast<int8_t>(key::W - key::S);

  if (sideways != 0 || forwards != 0) {
    player::speed = player::playerSpeedDefault;
  } else {
    player::speed = 0;
  }

  const auto speedMultiplier = static_cast<float>(0.0166 * static_cast<double>(player::speed) * utils::deltaTime);

  player::moveDirRad = atan2f(sideways, forwards);

  const float futureX = player::pos.x + cosf(player::rotRad + player::moveDirRad) * speedMultiplier;
  const float futureY = player::pos.y + sinf(player::rotRad + player::moveDirRad) * speedMultiplier;

  physics::PlayerCollisionCheck({futureX, futureY});
}
