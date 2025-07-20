// #if defined(_WIN32) || defined(_WIN64)
// #else
// #endif

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

#include "colors.h"
#include "config.h"
#include "controls.h"
#include "display.h"
#include "level.h"
#include "map_view.h"
#include "player.h"
#include "ray_caster.h"
#include "renderer/sdl.h"
#include "utils.h"

#include <cmath>
#include <cstdint>
#include <iostream>
#include <string>

using namespace std;

// extra debug stuff
bool limitSpeed = false;
// bool noiseEnabled = false;

// needed for calculations inside the loop

int64_t currentTime = utils::GetMicroTime();

#define WINDOW_TITLE_LENGTH 32
char windowTitle[WINDOW_TITLE_LENGTH];

void HandleDrawing() {

  display::pixels = sdl::GetPixelBuffer();

  // if (noiseEnabled) {
  //   for (int p = 0; p < display::size; p++) {
  //     pixels[p] = rand_uint32_t();
  //   }
  // }

  if (map_view::mapView) {
    for (int i = 0; i < display::size; i++) {
      display::pixels[i] = BLACK_COLOR;
    }
  } else {
    for (int i = 0; i < display::size; i++) {
      const float y = static_cast<float>(i) / static_cast<float>(display::width);

      // add ceiling/floor color
      if (y > static_cast<float>(display::height) / 2.0f + player::rotVerticalRad) {
        // pixels[i] = vga_palette[0x13];
        display::pixels[i] = GREY_COLOR;
      } else {
        // pixels[i] = vga_palette[0x12];
        display::pixels[i] = DARKER_GREY_COLOR;
      }
    }
  }

  ray_caster::CastRays();
  if (map_view::mapView) {
    map_view::DrawMap();
  }

  sdl::Draw();
}

void HandleTimings(const int64_t startTime) {
  using namespace utils;
  if (limitSpeed) {
    const int executionTime = static_cast<int>(GetMicroTime() - startTime);
    const int timeToSleep = 16666 - executionTime;

    if (timeToSleep > 0) {
      Sleep(timeToSleep);
    }
  }

  const int elapsedTime = static_cast<int>(GetMicroTime() - currentTime);
  const int executionTimeWithSleep = static_cast<int>(GetMicroTime() - startTime);

  int const avgFps = CalculateAverageFps(executionTimeWithSleep);

#ifndef __EMSCRIPTEN__ // delta time just doesn't work in emscripten as expected
  deltaTime = static_cast<double>(GetMicroTime() - startTime) * 60.0 / 1000000.0;
  // disable delta time if fps is too low to prevent clipping across walls
  if (deltaTime > 2.5f) {
    deltaTime = 1.0f;
  }
#endif

  // 1 million microsecond
  if (elapsedTime >= 1000000) {
    snprintf(windowTitle, WINDOW_TITLE_LENGTH, "%dx%d - %d fps", display::width, display::height, avgFps);
    sdl::SetWindowTitle(windowTitle);
    currentTime = GetMicroTime();
  }
}

void GameLoop() {
  if (!utils::running) {
    sdl::Quit();
  }

  // start time is used to calculate delta time
  const int64_t startTime = utils::GetMicroTime();

  controls::HandleControls();
  HandleDrawing();
  HandleTimings(startTime);
}

int main(int, char **) {
  const auto cfg = config::ReadConfigFile();

  level::LoadLevel("level1");

  if (cfg.retroResolution) {
    display::resScale = 1.0f / (480.0f / static_cast<float>(cfg.height));
    display::width = 640;
    display::height = 480;
  } else {
    display::resScale = 1.0f / (cfg.resolutionPercentage / 100.0f);
    display::width = static_cast<int>(round(static_cast<float>(cfg.width) / display::resScale));
    display::height = static_cast<int>(round(static_cast<float>(cfg.height) / display::resScale));
  }

  display::size = display::width * display::height;

  const int result = sdl::InitSDL(cfg.fullscreen, cfg.width, cfg.height, cfg.linearFiltering);
  if (result != 0) {
    return 1;
  }

  ray_caster::multiThreaded = cfg.multiThreaded;

#ifdef __EMSCRIPTEN__
  ray_caster::multiThreaded = false; // it doesn't work in emscripten yet
  cout << "Starting emscripten main loop..." << endl;
  emscripten_set_main_loop(GameLoop, 0, 1);
#else
  cout << "Starting main loop..." << endl;
  while (utils::running) {
    GameLoop();
  }
#endif

  cout << "Closing game..." << endl;
  return 0;
}
