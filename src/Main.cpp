// #if defined(_WIN32) || defined(_WIN64)
// #else
// #endif

#include <SDL2/SDL.h>

#include <cmath>
#include <cstdint>
#include <iostream>
#include <string>

#include "Colors.h"
#include "ConfigReader.h"
#include "Display.h"
#include "ExtraMath.h"
#include "Map2D.h"
#include "Player.h"
#include "RayCaster.h"
#include "Shapes.h"
#include "Structs.h"
#include "TextureLoader.h"
#include "Utils.h"

using std::cout;
using std::endl;
using std::string;
using std::to_string;

Config cfg = ReadConfigFile();

int windowWidth = cfg.width;
int windowHeight = cfg.height;

float resScale = 1.0f / ((float)cfg.resolutionPercentage / 100.0f);
int width = round(windowWidth / resScale);
int height = round(windowHeight / resScale);

int size = width * height;

SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;
SDL_Texture *texture = NULL;
SDL_Event event;

DisplayData dd;
Player player;

uint32_t *tileMap = NULL;

// clang-format off
int8_t map[16 * 16] = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 ,1, 1, 1,
                        1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
                        1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1,
                        1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
                        1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
                        1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
                        1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 1,
                        1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
                        1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 1,
                        1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
                        1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 1,
                        1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
                        1, 0, 0, 0, 0, 2, 2, 2, 2, 0, 0, 0, 0, 0, 0, 1,
                        1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
                        1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1
                      };
// clang-format on

float playerSpeedDefault = 4.0f;

// key pressed values
int8_t wKeyPressed = 0;
int8_t sKeyPressed = 0;
int8_t aKeyPressed = 0;
int8_t dKeyPressed = 0;

// bool tabKeyPressed = false;

// bool nKeyPressed = false;
// bool hKeyPressed = false;

// map
bool mapEnabled = false;

// extra debug stuff
bool limitSpeed = false;
bool noiseEnabled = false;

// needed for calculations inside the loop
float deltaTime = 1.0f;
long currentTime = GetMicroTime();

bool running = true;

void InitSDL() {
  int windowMode = SDL_WINDOW_SHOWN;
  if (cfg.fullscreen) {
    windowMode = SDL_WINDOW_FULLSCREEN;
  }

  tileMap = LoadTexture("tilemap", 512, 512);
  if (tileMap == NULL) {
    throw("Tilemap texture coulnd't be loaded");
  }
  // uint32_t *skybox1 = LoadTexture("skybox1", 512, 256);

  // initialize sdl
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    throw(SDL_GetError());
  }
  // create window
  window = SDL_CreateWindow("", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, windowWidth, windowHeight, windowMode);
  if (window == NULL) {
    SDL_Quit();
    throw(SDL_GetError());
  }

  // SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);

  // create renderer
  renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
  if (renderer == NULL) {
    SDL_DestroyWindow(window);
    SDL_Quit();
    throw(SDL_GetError());
  }

  // set resolution inside the window
  SDL_RenderSetLogicalSize(renderer, width, height);
  if (cfg.linearFiltering) {
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "2");
  }

  // create the texture that will display content in the window
  texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGB888, SDL_TEXTUREACCESS_STREAMING, width, height);
  if (texture == NULL) {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    throw(SDL_GetError());
  }

  SDL_SetRelativeMouseMode(SDL_TRUE);
}

void InitValues() {
  dd.width = width;
  dd.height = height;
  dd.size = size;

  // player values
  player.pos.x = 8.0f;
  player.pos.y = 8.0f;
}

void Quit() {
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();
}

void GameLoop() {
  if (!running) {
    Quit();
  }

  // start time is used to calculate delta time
  long startTime = GetMicroTime();

  // handle events
  while (SDL_PollEvent(&event)) {
    switch (event.type) {
    case SDL_QUIT:
      running = false;
      break;
    // look for a keypress
    case SDL_KEYDOWN:
      if (event.key.keysym.sym == SDLK_ESCAPE) {
        running = false;
      }
      if (event.key.keysym.sym == SDLK_w) {
        wKeyPressed = 1;
      }
      if (event.key.keysym.sym == SDLK_s) {
        sKeyPressed = 1;
      }
      if (event.key.keysym.sym == SDLK_a) {
        aKeyPressed = 1;
      }
      if (event.key.keysym.sym == SDLK_d) {
        dKeyPressed = 1;
      }
      if (event.key.keysym.sym == SDLK_TAB) {
        ToggleMap(&mapEnabled);
      }
      if (event.key.keysym.sym == SDLK_n) {
        noiseEnabled = !noiseEnabled;
      }
      break;
    case SDL_KEYUP:
      if (event.key.keysym.sym == SDLK_w) {
        wKeyPressed = 0;
      }
      if (event.key.keysym.sym == SDLK_s) {
        sKeyPressed = 0;
      }
      if (event.key.keysym.sym == SDLK_a) {
        aKeyPressed = 0;
      }
      if (event.key.keysym.sym == SDLK_d) {
        dKeyPressed = 0;
      }
      break;
    case SDL_MOUSEMOTION:
      player.rotRad += deg2rad(event.motion.xrel) * resScale / 8;

      if (player.rotRad < -M_PI) {
        player.rotRad += 2 * M_PI;
      } else if (player.rotRad > M_PI) {
        player.rotRad -= 2 * M_PI;
      }

      player.rotDeg = player.rotRad * 57.29578;

      // player.rot = -player.rot;

      // mousePosition.x = event.motion.x;
      // mousePosition.y = event.motion.y;

      // printf("%.6f\n", player.rot);

      break;
    }
  }

  const int8_t sideways = dKeyPressed - aKeyPressed;
  const int8_t forwards = wKeyPressed - sKeyPressed;

  if (sideways != 0 || forwards != 0) {
    player.speed = playerSpeedDefault;
  } else {
    player.speed = 0;
  }

  const float speedMultiplier = 0.0166f * player.speed * deltaTime;

  player.moveDirRad = atan2f(sideways, forwards);

  const int colX = player.pos.x + cosf(player.rotRad + player.moveDirRad) / 2.0f;
  const int colY = player.pos.y + sinf(player.rotRad + player.moveDirRad) / 2.0f;

  const int i = colY * 16 + colX;
  if (i < 256) {
    if (map[i] == 0) {
      player.pos.x += cosf(player.rotRad + player.moveDirRad) * speedMultiplier;
      player.pos.y += sinf(player.rotRad + player.moveDirRad) * speedMultiplier;
    }
  }

  // lock the texture
  uint32_t *pixels;
  int pitch;
  if (SDL_LockTexture(texture, NULL, (void **)&pixels, &pitch) != 0) {
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    printf("SDL_LockTexture Error: %s\n", SDL_GetError());
    SDL_Quit();
  }
  dd.pixels = pixels;

  // draw stuff before casting rays
  for (int i = 0; i < size; i++) {
    // const int x = i % width;
    const int y = i / width;

    // add ceiling/floor color
    if (y > height / 2) {
      pixels[i] = GREY_COLOR;
    } else {
      pixels[i] = DARKER_GREY_COLOR;
    }

    // draw sky
    // const float skyboxScale = 256.0f / (height / 2.0f);

    // const int skyboxX = x * skyboxScale + player.rotRad * (player.rotRad * 57.29578f);
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

  if (noiseEnabled) {
    for (int p = 0; p < size; p++) {
      pixels[p] = rand();
    }
  }
  CastRays(&dd, &player, map, tileMap);
  if (mapEnabled)
    DrawMap(&dd, map, &player);

  // unlock the texture and render the scene
  SDL_UnlockTexture(texture);
  SDL_RenderCopy(renderer, texture, NULL, NULL);
  SDL_RenderPresent(renderer);

  // printf("execution time: %d\n", executionTime);
  if (limitSpeed) {
    int executionTime = GetMicroTime() - startTime;
    int timeToSleep = 16666 - executionTime;

    if (timeToSleep > 0) {
      Sleep(timeToSleep);
    }
  }

  long const elapsedTime = GetMicroTime() - currentTime;
  long const executionTimeWithSleep = GetMicroTime() - startTime;

  int const avgFps = CalculateAverageFps(executionTimeWithSleep);
  deltaTime = (GetMicroTime() - startTime) * 60.0f / 1000000.0f;

  // 1 million microsecond
  if (elapsedTime >= 1000000) {
    const string title = to_string(width) + "x" + to_string(height) + " - " + to_string(avgFps) + " fps";
    SDL_SetWindowTitle(window, title.c_str());
    currentTime = GetMicroTime();
  }
}

int main(int, char **) {
  InitSDL();
  InitValues();

  while (running) {
    GameLoop();
  }

  return 0;
}
