#include <SDL2/SDL.h>
#include <SDL2/SDL_log.h>
#include <SDL2/SDL_pixels.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_video.h>
#include <SDL_events.h>
#include <SDL_keycode.h>
#include <SDL_stdinc.h>
#include <bits/stdint-intn.h>
#include <bits/stdint-uintn.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#include "ProToMath.h"

#define WHITE_COLOR 0XFFFFFF
#define BLACK_COLOR 0X000000
#define RED_COLOR 0XFF0000
#define GREEN_COLOR 0X00FF00
#define BLUE_COLOR 0X0000FF

#define macro()

struct Vector2 {
  float x;
  float y;
};

struct Vector2i {
  int x;
  int y;
};

// int randomColor() {
//   // Seed the random number generator (important)
//   srand(time(NULL));

//   int random_value = rand();

//   int color1 = random_value & 0xFF;
//   int color2 = (random_value >> 8) & 0xFF;
//   int mask = 0x3F;
//   int color3 = (random_value ^ mask) & 0xFF;
// }

long getMicrotime() {
  struct timeval currentTime;
  gettimeofday(&currentTime, NULL);
  return currentTime.tv_sec * (int)1e6 + currentTime.tv_usec;
}

void AddPixelToBuffer(int x, int y, int width, int height, int *pixelBuffer) {
  if (x <= 0 || x >= width || y <= 0 || y >= height) {
  } else {
    pixelBuffer[y * width + x] = WHITE_COLOR;
  }
}

void addCircle(float radius, struct Vector2i circlePos, int width, int height,
               int *pixelBuffer) {
  // struct Vector2i points[360];

  for (int i = 0; i < 360; i++) {
    float angle = (float)i;

    float yf = radius * sinus(deg2rad(angle));
    float xf = sqroot(sqr(radius) - sqr(yf));

    int x;
    int y;

    // int prevX = -1;
    // int prevY = -1;

    if (angle >= 0.0f && angle < 90.0f) {
      x = circlePos.x + roundToInt(xf);
      y = circlePos.y + roundToInt(yf);
    } else if (angle >= 90.0f && angle < 180.0f) {
      x = circlePos.x + roundToInt(xf);
      y = circlePos.y - roundToInt(yf);
    } else if (angle >= 180.0f && angle < 270.f) {
      x = circlePos.x - roundToInt(xf);
      y = circlePos.y - roundToInt(yf);
    } else if (angle >= 270.0f && angle <= 360.0f) {
      x = circlePos.x - roundToInt(xf);
      y = circlePos.y + roundToInt(yf);
    } else {
      printf("wrong angle\n");
      continue;
    }
    // printf("X: %d, Y: %d\n", circlePos.x, circlePos.y);

    AddPixelToBuffer(x, y, width, height, pixelBuffer);
    AddPixelToBuffer(circlePos.x, circlePos.y, width, height, pixelBuffer);

    // prevX = x;
    // prevY = y;
  }
}

void fillWithNoise(int width, int height, int *pixelBuffer) {
  for (int y = 0; y < height; ++y) {
    for (int x = 0; x < width; ++x) {
      int random_value = rand();

      int r = random_value & 0xFF;
      int g = (random_value >> 8) & 0xFF;
      int mask = 0x00FFFFFF;
      int b = (random_value ^ mask) & 0xFF;

      // Uint8 r = rand() % 256;
      // Uint8 g = rand() % 256;
      // Uint8 b = rand() % 256;
      uint32_t color = (r << 16) | (g << 8) | b;

      pixelBuffer[y * width + x] = color;
    }
  }
}

void renderSW(SDL_Renderer *renderer, int width, int height, int *pixelBuffer) {
  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
  SDL_RenderClear(renderer);
  for (int x = 0; x < width; ++x) {
    for (int y = 0; y < height; ++y) {
      if (pixelBuffer[y * width + x] == 0) {
        continue;
      }
      uint32_t color = pixelBuffer[y * width + x];
      Uint8 r = color >> 16;
      Uint8 g = (color >> 8) & 0xFF;
      Uint8 b = color & 0xFF;
      SDL_SetRenderDrawColor(renderer, r, g, b, 255);
      SDL_RenderDrawPoint(renderer, x, y);
    }
  }
  SDL_RenderPresent(renderer);
}

int renderHW(SDL_Renderer *renderer, SDL_Window *window,
             SDL_Texture *texture_buffer, int width, int height,
             int *pixelBuffer) {
  void *pixels;
  int pitch;

  // lock texture
  if (SDL_LockTexture(texture_buffer, NULL, &pixels, &pitch) != 0) {
    SDL_DestroyTexture(texture_buffer);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    printf("SDL_LockTexture Error: %s\n", SDL_GetError());
    SDL_Quit();
    return 1;
  }

  uint32_t *pixPtr = (uint32_t *)pixels;
  for (int x = 0; x < width; x++) {
    for (int y = 0; y < height; y++) {
      int i = y * width + x;
      pixPtr[i] = pixelBuffer[i];
    }
  }
  SDL_UnlockTexture(texture_buffer);
  SDL_RenderCopy(renderer, texture_buffer, NULL, NULL);
  SDL_RenderPresent(renderer);

  return 0;
}

void resetPixelBuffer(int width, int height, int *pixelBuffer) {
  for (int x = 0; x < width; x++) {
    for (int y = 0; y < height; y++) {
      pixelBuffer[y * width + x] = 0;
    }
  }
}

void addPlayer(struct Vector2i playerPosMap, int width, int height,
               int *pixelBuffer) {
  float radius = 128.0f;
  addCircle(radius, playerPosMap, width, height, pixelBuffer);
}

int main() {
  int windowWidth = 1920;
  int windowHeight = 1080;

  int resScale = 1;

  int width = windowWidth / resScale;
  int height = windowHeight / resScale;

  // Initialize SDL
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    SDL_Log("SDL could not initialize! SDL_Error: %s", SDL_GetError());
    return 1;
  }

  // Create window
  SDL_Window *window =
      SDL_CreateWindow("", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                       windowWidth, windowHeight, SDL_WINDOW_SHOWN);
  if (window == NULL) {
    SDL_Log("Window could not be created! SDL_Error: %s", SDL_GetError());
    SDL_Quit();
    return 1;
  }

  // Create renderer
  SDL_Renderer *renderer =
      SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
  if (renderer == NULL) {
    SDL_Log("Renderer could not be created! SDL_Error: %s", SDL_GetError());
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 1;
  }

  // Set resolution inside the window
  SDL_RenderSetLogicalSize(renderer, width, height);

  // Create the texture that will display content in the window
  SDL_Texture *texture =
      SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGB888,
                        SDL_TEXTUREACCESS_STREAMING, width, height);
  if (texture == NULL) {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    printf("SDL_CreateTexture Error: %s\n", SDL_GetError());
    SDL_Quit();
    return 1;
  }

  // Create a 2D array that will store colors of each pixel
  printf("Array size to allocate: %d\n", width * height);
  int *pixelBuffer = malloc(width * height * sizeof(int));
  if (pixelBuffer == NULL) {
    perror("Failed to allocate memory");
    return EXIT_FAILURE;
  }

  // Access elements using row and column indices
  // for (size_t i = 0; i < width; i++) {
  //   for (size_t j = 0; j < height; j++) {
  //     pixelBuffer[i * height + j] = 0;  // Example assignment
  //   }
  // }

  // RNG
  srand(time(0));

  // Player values
  struct Vector2 playerPos = {0.0, 0.0};
  struct Vector2i playerPosMap = {playerPos.x, playerPos.y};
  float playerSpeed = 512.0f;

  // Key pressed values
  bool upKeyPressed = false;
  bool downKeyPressed = false;
  bool leftKeyPressed = false;
  bool rightKeyPressed = false;

  bool nKeyPressed = false;
  bool hKeyPressed = false;

  // Extra debug stuff
  bool limitSpeed = false;
  bool checkerBoardRendering = false;
  bool hardwareAcceleration = true;
  bool noiseEnabled = false;

  // needed for calculations inside the loop
  float deltaTime;
  long currentTime = getMicrotime();

  SDL_Event event;
  bool running = true;
  while (running) {
    // start time is used to calculate delta time
    long startTime = getMicrotime();

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
        if (event.key.keysym.sym == SDLK_UP) {
          upKeyPressed = true;
        }
        if (event.key.keysym.sym == SDLK_DOWN) {
          downKeyPressed = true;
        }
        if (event.key.keysym.sym == SDLK_LEFT) {
          leftKeyPressed = true;
        }
        if (event.key.keysym.sym == SDLK_RIGHT) {
          rightKeyPressed = true;
        }
        if (event.key.keysym.sym == SDLK_n) {
          noiseEnabled = !noiseEnabled;
        }
        if (event.key.keysym.sym == SDLK_h) {
          hardwareAcceleration = !hardwareAcceleration;
        }
        break;
      case SDL_KEYUP:
        if (event.key.keysym.sym == SDLK_UP) {
          upKeyPressed = false;
        }
        if (event.key.keysym.sym == SDLK_DOWN) {
          downKeyPressed = false;
        }
        if (event.key.keysym.sym == SDLK_LEFT) {
          leftKeyPressed = false;
        }
        if (event.key.keysym.sym == SDLK_RIGHT) {
          rightKeyPressed = false;
        }
        break;
      }
    }

    struct Vector2 inputDirection = {0.0f, 0.0f};
    inputDirection.x = rightKeyPressed - leftKeyPressed;
    inputDirection.y = upKeyPressed - downKeyPressed;

    const float value = 0.0166f * playerSpeed * deltaTime;
    playerPos.x = playerPos.x + value * inputDirection.x;
    playerPos.y = playerPos.y + value * inputDirection.y;

    // printf("X: %f, Y:%f\n", playerPos.x, playerPos.y);

    playerPosMap.x = roundToInt(playerPos.x);
    playerPosMap.y = -roundToInt(playerPos.y);

    // printf("XM: %d, YM:%d\n", playerPosMap.x, playerPosMap.y);

    // uint32_t *upixels = (uint32_t*)pixels;
    // for (int y = 0; y < height; ++y) {
    //     for (int x = 0; x < width; ++x) {
    //         Uint8 r = rand() % 256;
    //         Uint8 g = rand() % 256;
    //         Uint8 b = rand() % 256;
    //         Uint8 a = 255; // Full opacity
    //         uint32_t color = (r << 24) | (g << 16) | (b << 8) | a;
    //         upixels[y * (pitch / 4) + x] = color;
    //         // uint32_t color = 0xFF0000FF; // Example: red color with full
    //         opacity
    //         // upixels[y * (pitch / 4) + x] = color;
    //     }
    // }

    // reset pixel buffer
    resetPixelBuffer(width, height, pixelBuffer);

    if (noiseEnabled)
      fillWithNoise(width, height, pixelBuffer);

    addPlayer(playerPosMap, width, height, pixelBuffer);

    if (hardwareAcceleration) {
      int r = renderHW(renderer, window, texture, width, height, pixelBuffer);
      if (r != 0) {
        return 1;
      }
    } else {
      renderSW(renderer, width, height, pixelBuffer);
    }

    if (limitSpeed) {
      long executionTime = getMicrotime() - startTime;
      int timeToSleep = 16666 - executionTime;

      if (timeToSleep > 0) {
        usleep(timeToSleep);
      }
    }

    long elapsedTime = getMicrotime() - currentTime;
    // printf("%.6ld\n", elapsedTime);

    deltaTime = (getMicrotime() - startTime) * 60.0 / 1000000.0;
    // printf("%.6f\n", deltaTime);
    long frameTime = getMicrotime() - startTime;
    float gameSpeed = ((1.0f / frameTime / 60.0f) * 1000000.0) * deltaTime;

    // Break the loop after 1 second (1,000,000 microseconds)
    if (elapsedTime >= 100000) {
      int duration = getMicrotime() - startTime;
      int fps = 1000000 / duration;

      char buffer[20]; // Adjust the size as needed
      sprintf(buffer, "%d", fps);

      SDL_SetWindowTitle(window, buffer);
      currentTime = getMicrotime();
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
