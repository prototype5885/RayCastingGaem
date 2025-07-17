// #if defined(_WIN32) || defined(_WIN64)
// #else
// #endif

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

#include <SDL2/SDL.h>

#include "colors.h"
#include "config.h"
#include "controls.h"
#include "display.h"
#include "level.h"
#include "map_view.h"
#include "player.h"
#include "ray_caster.h"
#include "utils.h"

#include <cmath>
#include <cstdint>
#include <iostream>
#include <string>

using namespace std;

SDL_Window *window = nullptr;
SDL_Renderer *renderer = nullptr;
SDL_Texture *sdlTexture = nullptr;

// extra debug stuff
bool limitSpeed = false;
// bool noiseEnabled = false;

// needed for calculations inside the loop

int64_t currentTime = utils::GetMicroTime();

#define WINDOW_TITLE_LENGTH 32
char windowTitle[WINDOW_TITLE_LENGTH];

int InitSDL(const config::Config cfg) {
  int windowMode = SDL_WINDOW_SHOWN;
  if (cfg.fullscreen) {
    windowMode = SDL_WINDOW_FULLSCREEN;
  }

  // initialize sdl
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    cerr << SDL_GetError();
    return 1;
  }
  // create window
  window = SDL_CreateWindow("", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, cfg.width, cfg.height, windowMode);
  if (window == nullptr) {
    SDL_Quit();
    cerr << SDL_GetError();
    return 1;
  }

  // SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);

  // create renderer
  renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
  if (renderer == nullptr) {
    SDL_DestroyWindow(window);
    SDL_Quit();
    cerr << SDL_GetError();
    return 1;
  }

  // set resolution inside the window
  SDL_RenderSetLogicalSize(renderer, display::width, display::height);
  if (cfg.linearFiltering) {
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "2");
  }

  // create the texture that will display content in the window
  sdlTexture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGB888, SDL_TEXTUREACCESS_STREAMING, display::width, display::height);
  if (sdlTexture == nullptr) {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    cerr << SDL_GetError();
    return 1;
  }

  SDL_SetRelativeMouseMode(SDL_TRUE);

  return 0;
}

void Quit() {
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();
}

void HandleDrawing() {
  // lock the texture
  uint32_t *pixels;
  int pitch;
  if (SDL_LockTexture(sdlTexture, nullptr, reinterpret_cast<void **>(&pixels), &pitch) != 0) {
    SDL_DestroyTexture(sdlTexture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    printf("SDL_LockTexture Error: %s\n", SDL_GetError());
    SDL_Quit();
  }
  display::pixels = pixels;

  // if (noiseEnabled) {
  //   for (int p = 0; p < display::size; p++) {
  //     pixels[p] = rand_uint32_t();
  //   }
  // }

  if (map_view::mapView) {
    for (int i = 0; i < display::size; i++) {
      pixels[i] = BLACK_COLOR;
    }
  } else {
    for (int i = 0; i < display::size; i++) {
      const float y = static_cast<float>(i) / static_cast<float>(display::width);

      // add ceiling/floor color
      if (y > static_cast<float>(display::height) / 2.0f + player::rotVerticalRad) {
        // pixels[i] = vga_palette[0x13];
        pixels[i] = GREY_COLOR;
      } else {
        // pixels[i] = vga_palette[0x12];
        pixels[i] = DARKER_GREY_COLOR;
      }
    }
  }

  ray_caster::CastRays();
  if (map_view::mapView) {
    map_view::DrawMap();
  }

  // unlock the texture and render the scene
  SDL_UnlockTexture(sdlTexture);
  SDL_RenderCopy(renderer, sdlTexture, nullptr, nullptr);
  SDL_RenderPresent(renderer);
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
    SDL_SetWindowTitle(window, windowTitle);
    currentTime = GetMicroTime();
  }
}

void GameLoop() {
  if (!utils::running) {
    Quit();
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

  cout << "Initializing SDL..." << endl;
  const int result = InitSDL(cfg);
  if (result != 0) {
    return 1;
  }

#ifdef __EMSCRIPTEN__
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
