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
#include "ray_caster.h"
#include "utils.h"

#include <cmath>
#include <cstdint>
#include <format>
#include <iostream>
#include <string>

using namespace std;

SDL_Window *window = nullptr;
SDL_Renderer *renderer = nullptr;
SDL_Texture *sdlTexture = nullptr;

int logicalWidth, logicalHeight;
float resScale;

// extra debug stuff
bool limitSpeed = false;
// bool noiseEnabled = false;

// needed for calculations inside the loop

int64_t currentTime = GetMicroTime();

int InitSDL(const Config cfg) {
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
  SDL_RenderSetLogicalSize(renderer, logicalWidth, logicalHeight);
  if (cfg.linearFiltering) {
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "2");
  }

  // create the texture that will display content in the window
  sdlTexture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGB888, SDL_TEXTUREACCESS_STREAMING, logicalWidth, logicalHeight);
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

  // draw stuff before casting rays
  for (int i = 0; i < display::size; i++) {
    // const int x = i % width;
    const int y = i / logicalWidth;

    // add ceiling/floor color
    if (y > logicalHeight / 2) {
      pixels[i] = vga_palette[0x13];
    } else {
      pixels[i] = vga_palette[0x12];
    }

    // draw sky
    // const float skyboxScale = 256.0f / (height / 2.0f);

    // const int skyboxX = x * skyboxScale + player::rotRad * (player::rotRad * 57.29578f);
    // const int skyboxY = y * skyboxScale;
    // pixels[i] = skybox1[skyboxY * 512 + skyboxX];

    // add floor
    // if (y > height / 2) {
    //   pixels[i] = GREY_COLOR;
    // }
  }

  // draw sky
  // for (int i = 0; i < 512 * 256; i++) {
  //   const int x = i % 512;
  //   const int y = i / 512;

  //   pixels[y * width + x] = skybox1[i];
  // }

  // if (noiseEnabled) {
  //   for (int p = 0; p < display::size; p++) {
  //     pixels[p] = rand_uint32_t();
  //   }
  // }
  CastRays();
  // if (map_view::mapEnabled)
  // map_view::DrawMap();

  // unlock the texture and render the scene
  SDL_UnlockTexture(sdlTexture);
  SDL_RenderCopy(renderer, sdlTexture, nullptr, nullptr);
  SDL_RenderPresent(renderer);
}

void HandleTimings(const int64_t startTime) {
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
#endif

  // 1 million microsecond
  if (elapsedTime >= 1000000) {
    const string title = format("{}x{} - {} fps", logicalWidth, logicalHeight, avgFps);
    SDL_SetWindowTitle(window, title.c_str());
    currentTime = GetMicroTime();
  }
}

void GameLoop() {
  if (!running) {
    Quit();
  }

  // start time is used to calculate delta time
  const int64_t startTime = GetMicroTime();

  controls::HandleControls();
  HandleDrawing();
  HandleTimings(startTime);
}

int main(int, char **) {
  const Config cfg = ReadConfigFile();

  LoadLevel("level1");

  if (cfg.retroResolution) {
    resScale = 1.0f / (480.0f / static_cast<float>(cfg.height));
    logicalWidth = 640;
    logicalHeight = 480;
  } else {
    resScale = 1.0f / (cfg.resolutionPercentage / 100.0f);
    logicalWidth = static_cast<int>(round(static_cast<float>(cfg.width) / resScale));
    logicalHeight = static_cast<int>(round(static_cast<float>(cfg.height) / resScale));
  }

  display::width = logicalWidth;
  display::height = logicalHeight;
  display::size = logicalWidth * logicalHeight;

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
  while (running) {
    GameLoop();
  }
#endif

  cout << "Closing game..." << endl;
  return 0;
}
