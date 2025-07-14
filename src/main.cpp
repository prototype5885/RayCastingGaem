// #if defined(_WIN32) || defined(_WIN64)
// #else
// #endif

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

#include <SDL2/SDL.h>

#include "colors.h"
#include "config.h"
#include "extra_math.h"
#include "level.h"
#include "map_view.h"
#include "player.h"
#include "ray_caster.h"
#include "structs.h"
#include "texture.h"
#include "utils.h"

#include <cmath>
#include <cstdint>
#include <string>

using namespace std;

SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;
SDL_Texture *sdlTexture = NULL;
SDL_Event event;

DisplayData dd;
Player player(8.0, 8.0, 0.0);

int logicalWidth, logicalHeight;
float resScale;

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

int InitSDL(Config cfg) {
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
  if (window == NULL) {
    SDL_Quit();
    cerr << SDL_GetError();
    return 1;
  }

  // SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);

  // create renderer
  renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
  if (renderer == NULL) {
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
  if (sdlTexture == NULL) {
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

void HandleControls() {
  while (SDL_PollEvent(&event)) {
    switch (event.type) {
    case SDL_QUIT:
      running = false;
      break;
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
    if (currentLevel[i] == 0) {
      player.pos.x += cosf(player.rotRad + player.moveDirRad) * speedMultiplier;
      player.pos.y += sinf(player.rotRad + player.moveDirRad) * speedMultiplier;
    }
  }
}

void HandleDrawing() {
  // lock the texture
  uint32_t *pixels;
  int pitch;
  if (SDL_LockTexture(sdlTexture, NULL, (void **)&pixels, &pitch) != 0) {
    SDL_DestroyTexture(sdlTexture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    printf("SDL_LockTexture Error: %s\n", SDL_GetError());
    SDL_Quit();
  }
  dd.pixels = pixels;

  // draw stuff before casting rays
  for (int i = 0; i < dd.size; i++) {
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
  CastRays(&dd, &player);
  if (mapEnabled)
    DrawMap(&dd, &player);

  // unlock the texture and render the scene
  SDL_UnlockTexture(sdlTexture);
  SDL_RenderCopy(renderer, sdlTexture, NULL, NULL);
  SDL_RenderPresent(renderer);
}

void HandleTimings(long startTime) {
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

#ifndef __EMSCRIPTEN__ // delta time just doesn't work in emscripten as expected
  deltaTime = (GetMicroTime() - startTime) * 60.0f / 1000000.0f;
#endif

  // 1 million microsecond
  if (elapsedTime >= 1000000) {
    const string title = to_string(logicalWidth) + "x" + to_string(logicalHeight) + " - " + to_string(avgFps) + " fps";
    SDL_SetWindowTitle(window, title.c_str());
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

int main(int, char **) {
  Config cfg = ReadConfigFile();

  LoadLevel("level1");

  if (cfg.retroResolution) {
    resScale = 1.0f / (480.0f / static_cast<float>(cfg.height));
    logicalWidth = 640;
    logicalHeight = 480;
  } else {
  resScale = 1.0f / (cfg.resolutionPercentage / 100.0f);
    logicalWidth = round(cfg.width / resScale);
    logicalHeight = round(cfg.height / resScale);
  }

  dd.width = logicalWidth;
  dd.height = logicalHeight;
  dd.size = logicalWidth * logicalHeight;

  cout << "Initializing SDL..." << endl;
  int result = InitSDL(cfg);
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
