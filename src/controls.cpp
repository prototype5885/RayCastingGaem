#include "controls.h"
#include "display.h"
#include "geometry.h"
#include "map_view.h"
#include "physics.h"
#include "player.h"
#include "utils.h"

#include <SDL_events.h>
#include <iostream>
#include <trigonometric.hpp>

#define VIEW_ANGLE_LIMIT 500.0f

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
      player::rotRad += glm::radians(static_cast<float>(event.motion.xrel)) * display::resScale / 8;

      if (player::rotRad < -M_PI) {
        player::rotRad += 2 * M_PI;
      } else if (player::rotRad > M_PI) {
        player::rotRad -= 2 * M_PI;
      }

      player::rotVerticalRad -= glm::radians(static_cast<float>(event.motion.yrel)) * display::resScale * 96;
      player::rotVerticalRad = glm::clamp(player::rotVerticalRad, -VIEW_ANGLE_LIMIT, VIEW_ANGLE_LIMIT);

      break;
    case SDL_MOUSEWHEEL:
      if (map_view::mapView) {
        map_view::ZoomMap(event.wheel.y);
      }
    default:;
    }
  }

  player::forwardVector = geometry::GetForwardVector(player::rotRad);

  const auto w = keyStates[SDL_SCANCODE_W];
  const auto a = keyStates[SDL_SCANCODE_A];
  const auto s = keyStates[SDL_SCANCODE_S];
  const auto d = keyStates[SDL_SCANCODE_D];

  const auto up = keyStates[SDL_SCANCODE_SPACE];
  const auto down = keyStates[SDL_SCANCODE_LCTRL];

  const auto sideways = d - a;
  const auto forwards = w - s;
  const auto vertical = up - down;

  player::z -= static_cast<float>(static_cast<double>(vertical) * 32.0f * utils::deltaTime);

  if (sideways != 0 || forwards != 0) {
    player::speed = player::playerSpeedDefault;
  } else {
    player::speed = 0;
  }

  const auto speedMultiplier = static_cast<float>(0.0166 * static_cast<double>(player::speed) * utils::deltaTime);

  player::moveDirRad = static_cast<float>(atan2(sideways, forwards));

  const float futureX = player::pos.x + cosf(player::rotRad + player::moveDirRad) * speedMultiplier;
  const float futureY = player::pos.y + sinf(player::rotRad + player::moveDirRad) * speedMultiplier;

  physics::PlayerCollisionCheck({futureX, futureY});

  // const float result = geometry::IsFacingTarget(player::pos, player::forwardVector, {0.0f, 0.0f});
  // std::cout << result << std::endl;
}
