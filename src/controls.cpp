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
#include <format>
#include <iostream>

SDL_Event event;
const uint8_t *keyStates = SDL_GetKeyboardState(nullptr);

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
      if (event.key.keysym.sym == SDLK_TAB) {
        map_view::ToggleMap();
      }

      // if (event.key.keysym.sym == SDLK_n) {
      //   noiseEnabled = !noiseEnabled;
      // }

      break;
    case SDL_MOUSEMOTION:
      player::rotRad += geometry::deg2rad(static_cast<float>(event.motion.xrel)) * display::resScale / 8;

      if (player::rotRad < -M_PI) {
        player::rotRad += 2 * M_PI;
      } else if (player::rotRad > M_PI) {
        player::rotRad -= 2 * M_PI;
      }

      player::rotVerticalRad -= geometry::deg2rad(static_cast<float>(event.motion.yrel)) * display::resScale * 96;
      player::rotVerticalRad = clamp(player::rotVerticalRad, -500.0f, 500.0f);

      break;
    case SDL_MOUSEWHEEL:
      if (map_view::mapView) {
        map_view::ZoomMap(event.wheel.y);
      }
    default:;
    }
  }

  const int8_t w = keyStates[SDL_SCANCODE_W] ? 1 : 0;
  const int8_t a = keyStates[SDL_SCANCODE_A] ? 1 : 0;
  const int8_t s = keyStates[SDL_SCANCODE_S] ? 1 : 0;
  const int8_t d = keyStates[SDL_SCANCODE_D] ? 1 : 0;

  const uint8_t up = keyStates[SDL_SCANCODE_SPACE] ? 1 : 0;
  const uint8_t down = keyStates[SDL_SCANCODE_LCTRL] ? 1 : 0;

  const auto sideways = d - a;
  const auto forwards = w - s;
  const auto vertical = up - down;

  player::z += static_cast<float>(static_cast<double>(vertical) * utils::deltaTime);

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
