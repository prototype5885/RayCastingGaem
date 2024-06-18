#include <SDL2/SDL.h>
#include <SDL2/SDL_log.h>
#include <SDL2/SDL_pixels.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_video.h>
#include <SDL_events.h>
#include <SDL_keycode.h>
#include <SDL_mouse.h>
#include <SDL_stdinc.h>
#include <bits/stdint-intn.h>
#include <bits/stdint-uintn.h>
#include <immintrin.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#include "ProToMath.h"

#define WHITE_COLOR (0 << 24) | (255 << 16) | (255 << 8) | 255
#define BLACK_COLOR (0 << 24) | (0 << 16) | (0 << 8) | 0
#define RED_COLOR (0 << 24) | (255 << 16) | (0 << 8) | 0
#define GREEN_COLOR (0 << 24) | (0 << 16) | (255 << 8) | 0
#define BLUE_COLOR (0 << 24) | (0 << 16) | (0 << 8) | 255

#define FPS_HISTORY_SIZE 32

#define COLOR_CHANNELS 3

#define macro()

typedef struct {
  float x, y;
} Vector2;

typedef struct {
  int x, y;
} Vector2i;

struct Player {
  Vector2 pos;
  float rotation;
};

typedef struct {
  uint8_t r, g, b;
} RGB;

typedef struct {
  int width, height;
  uint32_t *pixelBuffer;
} WindowData;

int map[8][8] = {{1, 1, 1, 1, 1, 1, 1, 1}, {1, 0, 0, 0, 0, 0, 0, 1},
                 {1, 0, 0, 0, 0, 1, 0, 1}, {1, 0, 0, 0, 0, 0, 0, 1},
                 {1, 0, 0, 0, 0, 0, 0, 1}, {1, 0, 0, 0, 0, 0, 0, 1},
                 {1, 1, 0, 0, 0, 0, 0, 1}, {1, 1, 1, 1, 1, 1, 1, 1}};

long GetMicroTime() {
  struct timeval currentTime;
  gettimeofday(&currentTime, NULL);
  return currentTime.tv_sec * (int)1e6 + currentTime.tv_usec;
}

uint32_t PackRGB(uint8_t r, uint8_t g, uint8_t b) {
  uint32_t argb = 0;
  argb |= 0 << 24;
  argb |= r << 16;
  argb |= g << 8;
  argb |= b;

  return argb;
}

RGB UnpackRGB(uint32_t packedRgb) {
  const uint8_t r = (packedRgb >> 16) & 0xFF;
  const uint8_t g = (packedRgb >> 8) & 0xFF;
  const uint8_t b = (packedRgb) & 0xFF;
  RGB rgb = {r, g, b};
  return rgb;
}

void AddPixelToBuffer(int width, int height, uint32_t *pixelBuffer, int x,
                      int y, uint32_t color) {
  if (x > 0 && x < width && y > 0 && y < height) {
    pixelBuffer[y * width + x] = color;
  }
}

void addCircle(int width, int height, uint32_t *pixelBuffer, float radius,
               Vector2i circlePos) {
  for (float i = 0; i < 360; i++) {
    const float angle = (float)i;

    const float yf = radius * sinf(deg2rad(i));
    const float xf = sqrtf(sqr(radius) - sqr(yf));

    int x, y;

    if (angle >= 0.0f && angle < 90.0f) {
      x = circlePos.x + round(xf);
      y = circlePos.y + roundf(yf);
    } else if (angle >= 90.0f && angle < 180.0f) {
      x = circlePos.x + roundf(xf);
      y = circlePos.y - roundf(yf);
    } else if (angle >= 180.0f && angle < 270.f) {
      x = circlePos.x - roundf(xf);
      y = circlePos.y - roundf(yf);
    } else if (angle >= 270.0f && angle <= 360.0f) {
      x = circlePos.x - roundf(xf);
      y = circlePos.y + roundf(yf);
    } else {
      printf("wrong angle\n");
      continue;
    }
    AddPixelToBuffer(width, height, pixelBuffer, x, y, WHITE_COLOR);
  }
  // dot in center
  AddPixelToBuffer(width, height, pixelBuffer, circlePos.x, circlePos.y,
                   WHITE_COLOR);
}

void FillWithNoise(int width, int height, uint32_t *pixelBuffer) {
  // for (int y = 0; y < height; y++) {
  //   for (int x = 0; x < width; x++) {
  //     uint32_t randomColor = random();
  //     pixelBuffer[y * width + x] = randomColor;
  //   }
  // }
  for (int p = 0; p < width * height; p++) {
    int x = p % width;
    int y = p / width;
    int i = y * width + x;

    uint32_t randomColor = random();
    pixelBuffer[i] = randomColor;
  }
}

void SoftwareRenderer(int width, int height, uint32_t *pixelBuffer,
                      SDL_Renderer *renderer) {
  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
  SDL_RenderClear(renderer);

  for (int p = 0; p < width * height; p++) {
    int x = p % width;
    int y = p / width;
    int i = y * width + x;

    if (pixelBuffer[i] == 0) {
      continue;
    }
    const RGB rgb = UnpackRGB(pixelBuffer[i]);
    SDL_SetRenderDrawColor(renderer, rgb.r, rgb.g, rgb.b, 255);
    SDL_RenderDrawPoint(renderer, x, y);
  }

  SDL_RenderPresent(renderer);
}

void HardwareRenderer(int width, int height, uint32_t *pixelBuffer,
                      SDL_Renderer *renderer, SDL_Window *window,
                      SDL_Texture *texture) {
  // uint32_t *pixels;
  // int pitch;

  // // lock texture
  // if (SDL_LockTexture(texture, NULL, (void **)&pixels, &pitch) != 0) {
  //   SDL_DestroyTexture(texture);
  //   SDL_DestroyRenderer(renderer);
  //   SDL_DestroyWindow(window);
  //   printf("SDL_LockTexture Error: %s\n", SDL_GetError());
  //   SDL_Quit();
  //   return 1;
  // }

  // uint32_t *pixPtr = (uint32_t *)pixels;

  // memcpy(pixPtr, pixelBuffer, width * height * sizeof(uint32_t));

  // for (int i = 0; i < width * height; i++) {
  //   int x = i % width;
  //   int y = i / width;

  //   pixPtr[y * width + x] = pixelBuffer[y * width + x];
  // }
  // printf("%d\n", pixelBuffer[0]);
  // printf("%d\n", pixPtr[0]);

  // SDL_UnlockTexture(texture);
  // SDL_RenderCopy(renderer, texture, NULL, NULL);
  // SDL_RenderPresent(renderer);

  SDL_UpdateTexture(texture, NULL, pixelBuffer, width * 4);
  SDL_RenderCopy(renderer, texture, NULL, NULL);
  SDL_RenderPresent(renderer);

  // return 0;
}

void ResetPixelBuffer(int width, int height, uint32_t *pixelBuffer) {
  for (int p = 0; p < width * height; p++) {
    int x = p % width;
    int y = p / width;
    int i = y * width + x;

    pixelBuffer[i] = 0;
  }
}

void AddPlayer(int width, int height, uint32_t *pixelBuffer,
               Vector2 playerPos) {
  Vector2i playerPosMap = {roundf(playerPos.x), -roundf(playerPos.y)};

  float radius = 128.0f;
  addCircle(width, height, pixelBuffer, radius, playerPosMap);
}

Vector2i StartingPositionForCentering(int width, int height, int objectWidth,
                                      int objectHeight) {
  Vector2i pos;
  pos.x = width / 2 - objectWidth / 2;
  pos.y = height / 2 - objectHeight / 2;
  return pos;
}

// struct Vector2i PositionInCorner(int width, int height, int objectWidth,
// int objectHeight)
// {
//   struct Vector2i position;
//   position.x = width / 2 - (objectWidth) / 2;
//   position.y = height / 2 - (objectHeight) / 2;
//   return position;
// }

void DrawMap(int width, int height, uint32_t *pixelBuffer, struct Player player,
             int resScale) {
  int upscaleMultiplier = 64 / resScale;
  int mapSize = 8;

  Vector2i playerPosMap = {roundf(player.pos.x) / resScale,
                           -roundf(player.pos.y) / resScale};

  Vector2i pos = StartingPositionForCentering(
      width, height, upscaleMultiplier * mapSize, upscaleMultiplier * mapSize);
  // struct Vector2i pos = {0, 0};

  pos.x = pos.x - playerPosMap.x;
  pos.y = pos.y - playerPosMap.y;

  int midX = width / 2;
  int midY = height / 2;

  for (int y = 0; y < upscaleMultiplier; y++) {
    for (int x = 0; x < upscaleMultiplier; x++) {
      for (int mapY = 0; mapY < mapSize; mapY++) {
        for (int mapX = 0; mapX < mapSize; mapX++) {
          int screenY = mapY * upscaleMultiplier + y + pos.y;
          int screenX = mapX * upscaleMultiplier + x + pos.x;

          int index = screenY * width + screenX;

          // map squares
          if (map[mapX][mapY] == 1) {
            AddPixelToBuffer(width, height, pixelBuffer, screenX, screenY,
                             BLUE_COLOR);
          } else {
            AddPixelToBuffer(width, height, pixelBuffer, screenX, screenY,
                             BLACK_COLOR);
          }

          // outlines
          if (x == 0 || x == upscaleMultiplier - 1 || y == 0 ||
              y == upscaleMultiplier - 1) {
            AddPixelToBuffer(width, height, pixelBuffer, screenX, screenY,
                             WHITE_COLOR);
          }
        }
      }
    }
  }

  // draw player in center
  const int playerSize = 8 / resScale;
  Vector2i centerDotPos =
      StartingPositionForCentering(width, height, playerSize, playerSize);

  for (int y = 0; y < playerSize; y++) {
    for (int x = 0; x < playerSize; x++) {
      pixelBuffer[(centerDotPos.y + y) * width + (centerDotPos.x + x)] =
          GREEN_COLOR;
    }
  }
}

int CalculateAverageFps(int executionTime) {

  // const int fpsHistorySize = 16;
  static int fpsHistory[FPS_HISTORY_SIZE];

  // calculate fps
  int momentFps = 1000000 / executionTime;

  for (int i = FPS_HISTORY_SIZE; i >= 0; i--) {
    const int nexti = i + 1;
    if (nexti <= FPS_HISTORY_SIZE) {
      fpsHistory[nexti] = fpsHistory[i];
    }
  }
  fpsHistory[0] = momentFps;

  int sumFps = 0;
  for (int i = 0; i < FPS_HISTORY_SIZE; i++) {
    sumFps += fpsHistory[i];
    // printf("%d: %d\n", i, fpsHistory[i]);
  }
  const int avgFps = sumFps / FPS_HISTORY_SIZE;
  return avgFps;
}

void ToggleHardwareAcceleration(bool *hardwareAcceleration, char *hwsw) {
  *hardwareAcceleration = !(*hardwareAcceleration);
  if (*hardwareAcceleration) {
    strcpy(hwsw, "HW");
  } else {
    strcpy(hwsw, "SW");
  }
}
void ToggleMap(bool *mapEnabled) { *mapEnabled = !(*mapEnabled); }

int main() {
  int windowWidth = 1920;
  int windowHeight = 1080;

  int centerX = 1920 / 2;
  int centerY = 1080 / 2;

  int resScale = 4;

  int width = windowWidth / resScale;
  int height = windowHeight / resScale;

  // initialize sdl
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    SDL_Log("SDL could not initialize! SDL_Error: %s", SDL_GetError());
    return EXIT_FAILURE;
  }

  // create window
  SDL_Window *window =
      SDL_CreateWindow("", SDL_WINDOWPOS_CENTERED, SDL_WINDOW_FULLSCREEN,
                       windowWidth, windowHeight, SDL_WINDOW_SHOWN);
  if (window == NULL) {
    SDL_Log("Window could not be created! SDL_Error: %s", SDL_GetError());
    SDL_Quit();
    return EXIT_FAILURE;
  }

  // SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);

  // create renderer
  SDL_Renderer *renderer =
      SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
  if (renderer == NULL) {
    SDL_Log("Renderer could not be created! SDL_Error: %s", SDL_GetError());
    SDL_DestroyWindow(window);
    SDL_Quit();
    return EXIT_FAILURE;
  }

  // set resolution inside the window
  SDL_RenderSetLogicalSize(renderer, width, height);

  // create the texture that will display content in the window
  SDL_Texture *texture =
      SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGB888,
                        SDL_TEXTUREACCESS_STREAMING, width, height);
  if (texture == NULL) {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    printf("SDL_CreateTexture Error: %s\n", SDL_GetError());
    SDL_Quit();
    return EXIT_FAILURE;
  }

  // create a 2D array that will store colors of each pixel
  uint32_t *pixelBuffer = malloc(width * height * sizeof(uint32_t));
  if (pixelBuffer == NULL) {
    perror("Failed to allocate memory");
    return EXIT_FAILURE;
  }

  // RNG
  srand(time(0));

  // player values
  struct Player player;
  player.pos.x = 50.0;
  player.pos.y = -50.0;

  float playerSpeed = 256.0f;

  // key pressed values
  bool upKeyPressed = false;
  bool downKeyPressed = false;
  bool leftKeyPressed = false;
  bool rightKeyPressed = false;

  bool tabKeyPressed = false;

  bool nKeyPressed = false;
  bool hKeyPressed = false;

  // map
  bool mapEnabled = false;

  // extra debug stuff
  bool limitSpeed = false;
  bool checkerBoardRendering = false;
  bool hardwareAcceleration = true;
  bool noiseEnabled = true;

  // needed for calculations inside the loop
  float deltaTime = 1.0f;
  long currentTime = GetMicroTime();

  // stuff for average fps calculation

  // for (int i = 0; i < fpsHistorySize; i++) {
  //   fpsHistory[i] = 0;
  // }

  char hwsw[] = "HW";

  SDL_ShowCursor(false);

  SDL_Event event;
  bool running = true;
  while (running) {
    // start time is used to calculate delta time
    long startTime = GetMicroTime();

    SDL_WarpMouseInWindow(window, centerX, centerY);

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
          upKeyPressed = true;
        }
        if (event.key.keysym.sym == SDLK_s) {
          downKeyPressed = true;
        }
        if (event.key.keysym.sym == SDLK_a) {
          leftKeyPressed = true;
        }
        if (event.key.keysym.sym == SDLK_d) {
          rightKeyPressed = true;
        }
        if (event.key.keysym.sym == SDLK_TAB) {
          ToggleMap(&mapEnabled);
        }
        if (event.key.keysym.sym == SDLK_n) {
          noiseEnabled = !noiseEnabled;
        }
        if (event.key.keysym.sym == SDLK_h) {
          ToggleHardwareAcceleration(&hardwareAcceleration, hwsw);
        }
        break;
      case SDL_KEYUP:
        if (event.key.keysym.sym == SDLK_w) {
          upKeyPressed = false;
        }
        if (event.key.keysym.sym == SDLK_s) {
          downKeyPressed = false;
        }
        if (event.key.keysym.sym == SDLK_a) {
          leftKeyPressed = false;
        }
        if (event.key.keysym.sym == SDLK_d) {
          rightKeyPressed = false;
        }
        break;
      case SDL_MOUSEMOTION:
        player.rotation += (width / 2.0 - event.motion.x) / 32;

        if (player.rotation < 0) {
          player.rotation = 360.0;
        } else if (player.rotation > 360.0) {
          player.rotation = 0;
        }

        // printf("%f\n", player.rotation);

        break;
      }
    }

    Vector2 inputDirection = {0.0f, 0.0f};
    inputDirection.x = rightKeyPressed - leftKeyPressed;
    inputDirection.y = upKeyPressed - downKeyPressed;

    const float value = 0.0166f * playerSpeed * deltaTime;
    player.pos.x = player.pos.x + value * inputDirection.x;
    player.pos.y = player.pos.y + value * inputDirection.y;

    // printf("X: %f, Y:%f\n", player.pos.x, player.pos.y);

    // printf("XM: %d, YM:%d\n", playerPosMap.x, playerPosMap.y);

    // reset pixel buffer
    // ResetPixelBuffer(width, height, pixelBuffer);

    if (noiseEnabled)
      FillWithNoise(width, height, pixelBuffer);

    // AddPlayer(width, height, pixelBuffer, playerPos);

    if (mapEnabled) {
      DrawMap(width, height, pixelBuffer, player, resScale);
    }

    if (hardwareAcceleration) {
      HardwareRenderer(width, height, pixelBuffer, renderer, window, texture);
      // if (r != 0) {
      //   return EXIT_FAILURE;
      // }
    } else {
      SoftwareRenderer(width, height, pixelBuffer, renderer);
    }

    int executionTime = GetMicroTime() - startTime;
    long elapsedTime = GetMicroTime() - currentTime;

    // printf("execution time: %d\n", executionTime);
    if (limitSpeed) {
      int timeToSleep = 16666 - executionTime;

      if (timeToSleep > 0) {
        usleep(timeToSleep);
      }
    }

    // int sumFps = 0;

    // calculate fps
    // int momentFps = 1000000 / executionTime;

    // for (int i = fpsHistorySize; i >= 0; i--) {
    //   fpsHistory[i + 1] = fpsHistory[i];
    // }
    // fpsHistory[0] = momentFps;

    // for (int i = 0; i < fpsHistorySize; i++) {
    //   sumFps += fpsHistory[i];
    //   // printf("%d: %d\n", i, fpsHistory[i]);
    // }
    int avgFps = CalculateAverageFps(executionTime);

    // printf("%.6ld\n", elapsedTime);

    deltaTime = (GetMicroTime() - startTime) * 60.0 / 1000000.0;

    // printf("%.6f\n", deltaTime);
    // long frameTime = GetMicroTime() - startTime;
    // float gameSpeed = ((1.0f / frameTime / 60.0f) * 1000000.0) * deltaTime;

    // Break the loop after 1 second (1,000,000 microseconds)
    if (elapsedTime >= 1000000) {
      // int duration = GetMicroTime() - startTime;
      // printf("duration: %d\n", duration);

      char buffer[40]; // Adjust the size as needed
      sprintf(buffer, "%d fps, %s", avgFps, hwsw);

      SDL_SetWindowTitle(window, buffer);
      currentTime = GetMicroTime();
    }
  }

  // Free the allocated memory
  free(pixelBuffer);

  // Clean up
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();

  return 0;
}
