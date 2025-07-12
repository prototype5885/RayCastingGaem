// #if defined(_WIN32) || defined(_WIN64)
// #else
// #endif

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

#include <SDL2/SDL.h>

#include "math.h"
#include "stdint.h"
#include "stdio.h"

#include "Colors.h"
#include "ConfigReader.h"
#include "ExtraMath.h"
#include "MapView.h"
#include "Player.h"
#include "RayCaster.h"
#include "Structs.h"
#include "TextureLoader.h"
#include "Utils.h"

SDL_Window *window;
SDL_Renderer *renderer;
SDL_Texture *texture;
SDL_Event event;

DisplayData dd;
Player player;

uint32_t *tileMap;

int width, height;
float resScale;

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

// key pressed values
int8_t wKeyPressed = 0;
int8_t sKeyPressed = 0;
int8_t aKeyPressed = 0;
int8_t dKeyPressed = 0;

// map
char mapEnabled = 0;

// extra debug stuff
char limitSpeed = 0;
char noiseEnabled = 0;

// needed for calculations inside the loop
float deltaTime = 1.0f;
long currentTime;

char running = 1;

int InitSDL(Config cfg) {
  int windowMode = SDL_WINDOW_SHOWN;
  if (cfg.fullscreen) {
    windowMode = SDL_WINDOW_FULLSCREEN;
  }

  tileMap = LoadTexture("tilemap", 512, 512);
  if (tileMap == NULL) {
    printf("Tilemap texture coulnd't be loaded\n");
    return 1;
  }
  // uint32_t *skybox1 = LoadTexture("skybox1", 512, 256);

  // initialize sdl
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    printf("%s\n", SDL_GetError());
    return 1;
  }
  // create window
  window = SDL_CreateWindow("", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, cfg.width, cfg.height, windowMode);
  if (window == NULL) {
    SDL_Quit();
    printf("%s\n", SDL_GetError());
    return 1;
  }

  // SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);

  // create renderer
  renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
  if (renderer == NULL) {
    SDL_DestroyWindow(window);
    SDL_Quit();
    printf("%s\n", SDL_GetError());
    return 1;
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
    printf("%s\n", SDL_GetError());
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

void HandleControls() {
  while (SDL_PollEvent(&event)) {
    switch (event.type) {
    case SDL_QUIT:
      running = 0;
      break;
    case SDL_KEYDOWN:
      if (event.key.keysym.sym == SDLK_ESCAPE) {
        running = 0;
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
      break;
    }
  }

  const int8_t sideways = dKeyPressed - aKeyPressed;
  const int8_t forwards = wKeyPressed - sKeyPressed;

  if (sideways != 0 || forwards != 0) {
    player.speed = PLAYER_SPEED_DEFAULT;
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
}

void HandleDrawing() {
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
  for (int i = 0; i < dd.size; i++) {
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
    for (int p = 0; p < dd.size; p++) {
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
}

void HandleTimings(long startTime) {
  if (limitSpeed) {
    int executionTime = GetMicroTime() - startTime;
    int timeToSleep = 16666 - executionTime;

    if (timeToSleep > 0) {
      Wait(timeToSleep);
    }
  }

  long const elapsedTime = GetMicroTime() - currentTime;
  long const executionTimeWithSleep = GetMicroTime() - startTime;

  int const avgFps = CalculateAverageFps(executionTimeWithSleep);

#ifndef __EMSCRIPTEN__ // delta time just doesn't work in emscripten as expected
  deltaTime = (GetMicroTime() - startTime) * 60.0f / 1000000.0f;
#endif
  // 1 million microsecond
  if (elapsedTime >= 1000000) {
    char windowTitle[32];
    snprintf(windowTitle, 32, "%d x %d - %d fps", width, height, avgFps);
    SDL_SetWindowTitle(window, windowTitle);
    currentTime = GetMicroTime();
  }
}

void GameLoop() {
  if (!running) {
    Quit();
  }

  // start time is used to calculate delta time
  long startTime = GetMicroTime();

  HandleControls();
  HandleDrawing();
  HandleTimings(startTime);
}

int main(int arg, char **args) {
  Config cfg = ReadConfigFile();

  resScale = 1.0f / ((float)cfg.resolutionPercentage / 100.0f);
  width = round(cfg.width / resScale);
  height = round(cfg.height / resScale);

  dd.width = width;
  dd.height = height;
  dd.size = width * height;

  // player values
  player = NewPlayer(8.0, 8.0, 0.0);

  currentTime = GetMicroTime();

  int result = InitSDL(cfg);
  if (result != 0) {
    return result;
  }

#ifdef __EMSCRIPTEN__
  emscripten_set_main_loop(GameLoop, 0, 1);
#else
  while (running) {
    GameLoop();
  }
#endif

  return 0;
}
