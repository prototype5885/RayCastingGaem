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
#include <immintrin.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#include "ProToMath.h"

#define WHITE_COLOR (0 << 24) | (255 << 16) | (255 << 8) | 255
#define BLACK_COLOR (0 << 24) | (0 << 16) | (0 << 8) | 255
#define RED_COLOR (0 << 24) | (255 << 16) | (0 << 8) | 0
#define GREEN_COLOR (0 << 24) | (0 << 16) | (255 << 8) | 0
#define BLUE_COLOR (0 << 24) | (0 << 16) | (255 << 8) | 255

#define COLOR_CHANNELS 3

#define macro()

struct Vector2
{
  float x;
  float y;
};

struct Vector2i
{
  int x;
  int y;
};

typedef struct
{
  uint8_t r;
  uint8_t g;
  uint8_t b;
} RGB;

int map[8][8] = {{1, 1, 1, 1, 1, 1, 1, 1},
                 {1, 0, 0, 0, 0, 0, 0, 1},
                 {1, 0, 0, 0, 0, 1, 0, 1},
                 {1, 0, 0, 0, 0, 0, 0, 1},
                 {1, 0, 0, 0, 0, 0, 0, 1},
                 {1, 0, 0, 0, 0, 0, 0, 1},
                 {1, 1, 0, 0, 0, 0, 0, 1},
                 {1, 1, 1, 1, 1, 1, 1, 1}};

long GetMicroTime()
{
  struct timeval currentTime;
  gettimeofday(&currentTime, NULL);
  return currentTime.tv_sec * (int)1e6 + currentTime.tv_usec;
}

uint32_t PackRGB(uint8_t r, uint8_t g, uint8_t b)
{
  uint32_t rgba = 0;
  rgba |= 0 << 24;
  rgba |= r << 16;
  rgba |= g << 8;
  rgba |= b;

  return rgba;
}

RGB UnpackRGB(uint32_t packedRgb)
{
  RGB rgb;
  rgb.r = (packedRgb >> 16) & 0xFF;
  rgb.g = (packedRgb >> 8) & 0xFF;
  rgb.b = (packedRgb) & 0xFF;
  return rgb;
}

void AddPixelToBuffer(int x, int y, int width, int height, uint32_t *pixelBuffer)
{
  if (x > 0 && x < width && y > 0 && y < height)
  {
    pixelBuffer[y * width + x] = WHITE_COLOR;
  }
}

void addCircle(float radius, struct Vector2i circlePos, int width, int height, uint32_t *pixelBuffer)
{
  // struct Vector2i points[360];

  for (float i = 0; i < 360; i++)
  {
    const float angle = (float)i;

    const float yf = radius * sinf(deg2rad(i));
    const float xf = sqrtf(sqr(radius) - sqr(yf));

    int x;
    int y;

    // int prevX = -1;
    // int prevY = -1;

    if (angle >= 0.0f && angle < 90.0f)
    {
      x = circlePos.x + round(xf);
      y = circlePos.y + roundf(yf);
    }
    else if (angle >= 90.0f && angle < 180.0f)
    {
      x = circlePos.x + roundf(xf);
      y = circlePos.y - roundf(yf);
    }
    else if (angle >= 180.0f && angle < 270.f)
    {
      x = circlePos.x - roundf(xf);
      y = circlePos.y - roundf(yf);
    }
    else if (angle >= 270.0f && angle <= 360.0f)
    {
      x = circlePos.x - roundf(xf);
      y = circlePos.y + roundf(yf);
    }
    else
    {
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

void FillWithNoise(int width, int height, uint32_t *pixelBuffer)
{
  for (int y = 0; y < height; y++)
  {

    for (int x = 0; x < width; x++)
    {
      uint32_t randomColor = random();
      pixelBuffer[y * width + x] = randomColor;
    }
  }
}

void SoftwareRenderer(SDL_Renderer *renderer, int width, int height, uint32_t *pixelBuffer)
{
  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
  SDL_RenderClear(renderer);

  for (int x = 0; x < width; x++)
  {
    for (int y = 0; y < height; y++)
    {
      int index = y * width + x;
      if (pixelBuffer[index] == 0)
      {
        continue;
      }
      const RGB rgb = UnpackRGB(pixelBuffer[index]);
      SDL_SetRenderDrawColor(renderer, rgb.r, rgb.g, rgb.b, 255);
      SDL_RenderDrawPoint(renderer, x, y);
    }
  }
  SDL_RenderPresent(renderer);
}

int HardwareRenderer(SDL_Renderer *renderer, SDL_Window *window, SDL_Texture *texture_buffer, int width, int height, uint32_t *pixelBuffer)
{
  uint32_t *pixels;
  int pitch;

  // lock texture
  if (SDL_LockTexture(texture_buffer, NULL, (void **)&pixels, &pitch) != 0)
  {
    SDL_DestroyTexture(texture_buffer);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    printf("SDL_LockTexture Error: %s\n", SDL_GetError());
    SDL_Quit();
    return 1;
  }

  uint32_t *pixPtr = (uint32_t *)pixels;
  // pixPtr = pixelBuffer;

  for (int x = 0; x < width; x++)
  {
    for (int y = 0; y < height; y++)
    {
      int i = y * width + x;
      pixPtr[i] = pixelBuffer[i];
    }
  }

  SDL_UnlockTexture(texture_buffer);
  SDL_RenderCopy(renderer, texture_buffer, NULL, NULL);
  SDL_RenderPresent(renderer);

  // SDL_UpdateTexture(texture_buffer, NULL, pixelBuffer, width * 4);
  // SDL_RenderCopy(renderer, texture_buffer, NULL, NULL);
  // SDL_RenderPresent(renderer);

  return 0;
}

void ResetPixelBuffer(int width, int height, uint32_t *pixelBuffer)
{
  for (int x = 0; x < width; x++)
  {
    for (int y = 0; y < height; y++)
    {

      pixelBuffer[y * width + x] = 0;
    }
  }
}

void AddPlayer(struct Vector2i playerPosMap, int width, int height, uint32_t *pixelBuffer)
{
  float radius = 128.0f;
  addCircle(radius, playerPosMap, width, height, pixelBuffer);
}

void DrawMap(int width, int height, uint32_t *pixelBuffer)
{
  int upscaleMultiplier = 32;
  int mapSize = 8;

  int posX = 0;
  int posY = 0;

  for (int x = 0; x < upscaleMultiplier; x++)
  {
    for (int y = 0; y < upscaleMultiplier; y++)
    {
      for (int mapX = 0; mapX < mapSize; mapX++)
      {
        for (int mapY = 0; mapY < mapSize; mapY++)
        {
          int index = (mapY * upscaleMultiplier + y + posY) * width + (mapX * upscaleMultiplier + x + posX);

          if (map[mapX][mapY] == 1)
          {
            pixelBuffer[index] = RED_COLOR;
          }
          else
          {
            pixelBuffer[index] = GREEN_COLOR;
          }

          if (x == 0 || x == upscaleMultiplier - 1 || y == 0 || y == upscaleMultiplier - 1)
          {
            pixelBuffer[index] = WHITE_COLOR;
          }
        }
      }
    }
  }
}

int main()
{
  int windowWidth = 1920;
  int windowHeight = 1080;

  int resScale = 4;

  int width = windowWidth / resScale;
  int height = windowHeight / resScale;

  // Initialize SDL
  if (SDL_Init(SDL_INIT_VIDEO) < 0)
  {
    SDL_Log("SDL could not initialize! SDL_Error: %s", SDL_GetError());
    return 1;
  }

  // Create window
  SDL_Window *window =
      SDL_CreateWindow("", SDL_WINDOWPOS_CENTERED, SDL_WINDOW_FULLSCREEN,
                       windowWidth, windowHeight, SDL_WINDOW_SHOWN);
  if (window == NULL)
  {
    SDL_Log("Window could not be created! SDL_Error: %s", SDL_GetError());
    SDL_Quit();
    return 1;
  }

  // SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);

  // Create renderer
  SDL_Renderer *renderer =
      SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
  if (renderer == NULL)
  {
    SDL_Log("Renderer could not be created! SDL_Error: %s", SDL_GetError());
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 1;
  }

  // Set resolution inside the window
  SDL_RenderSetLogicalSize(renderer, width, height);

  // Create the texture that will display content in the window
  SDL_Texture *texture =
      SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGB888, SDL_TEXTUREACCESS_STREAMING, width, height);
  if (texture == NULL)
  {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    printf("SDL_CreateTexture Error: %s\n", SDL_GetError());
    SDL_Quit();
    return 1;
  }

  // Create a 2D array that will store colors of each pixel
  printf("Array size to allocate: %d\n", width * height);
  uint32_t *pixelBuffer = malloc(width * height * sizeof(uint32_t));
  if (pixelBuffer == NULL)
  {
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
  long currentTime = GetMicroTime();

  char hwsw[] = "HW";

  SDL_Event event;
  bool running = true;
  while (running)
  {
    // start time is used to calculate delta time
    long startTime = GetMicroTime();

    // handle events
    while (SDL_PollEvent(&event))
    {
      switch (event.type)
      {
      case SDL_QUIT:
        running = false;
        break;
      // look for a keypress
      case SDL_KEYDOWN:
        if (event.key.keysym.sym == SDLK_ESCAPE)
        {
          running = false;
        }
        if (event.key.keysym.sym == SDLK_UP)
        {
          upKeyPressed = true;
        }
        if (event.key.keysym.sym == SDLK_DOWN)
        {
          downKeyPressed = true;
        }
        if (event.key.keysym.sym == SDLK_LEFT)
        {
          leftKeyPressed = true;
        }
        if (event.key.keysym.sym == SDLK_RIGHT)
        {
          rightKeyPressed = true;
        }
        if (event.key.keysym.sym == SDLK_n)
        {
          noiseEnabled = !noiseEnabled;
        }
        if (event.key.keysym.sym == SDLK_h)
        {
          hardwareAcceleration = !hardwareAcceleration;
          if (hardwareAcceleration)
          {
            strcpy(hwsw, "HW");
          }
          else
          {
            strcpy(hwsw, "SW");
          }
        }
        break;
      case SDL_KEYUP:
        if (event.key.keysym.sym == SDLK_UP)
        {
          upKeyPressed = false;
        }
        if (event.key.keysym.sym == SDLK_DOWN)
        {
          downKeyPressed = false;
        }
        if (event.key.keysym.sym == SDLK_LEFT)
        {
          leftKeyPressed = false;
        }
        if (event.key.keysym.sym == SDLK_RIGHT)
        {
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

    playerPosMap.x = roundf(playerPos.x);
    playerPosMap.y = -roundf(playerPos.y);

    printf("XM: %d, YM:%d\n", playerPosMap.x, playerPosMap.y);

    // reset pixel buffer
    ResetPixelBuffer(width, height, pixelBuffer);

    if (noiseEnabled)
      FillWithNoise(width, height, pixelBuffer);

    AddPlayer(playerPosMap, width, height, pixelBuffer);

    DrawMap(width, height, pixelBuffer);

    // int size = 64;
    // for (int s = 0; s < 4; s++)
    // {
    //   for (int x = 0; x < size; x++)
    //   {
    //     for (int y = 0; y < size; y++)
    //     {
    //       int index = y * size + x;
    //       pixelBuffer[(s * size + y) * width + (s +x)] = WHITE_COLOR;
    //     }
    //   }
    // }

    // pixelBuffer[100 * width + 100] = WHITE_COLOR;

    if (hardwareAcceleration)
    {
      int r = HardwareRenderer(renderer, window, texture, width, height,
                               pixelBuffer);
      if (r != 0)
      {
        return 1;
      }
    }
    else
    {
      SoftwareRenderer(renderer, width, height, pixelBuffer);
    }

    if (limitSpeed)
    {
      long executionTime = GetMicroTime() - startTime;
      int timeToSleep = 16666 - executionTime;

      if (timeToSleep > 0)
      {
        usleep(timeToSleep);
      }
    }

    long elapsedTime = GetMicroTime() - currentTime;
    // printf("%.6ld\n", elapsedTime);

    deltaTime = (GetMicroTime() - startTime) * 60.0 / 1000000.0;
    // printf("%.6f\n", deltaTime);
    long frameTime = GetMicroTime() - startTime;
    float gameSpeed = ((1.0f / frameTime / 60.0f) * 1000000.0) * deltaTime;

    // Break the loop after 1 second (1,000,000 microseconds)
    if (elapsedTime >= 100000)
    {
      int duration = GetMicroTime() - startTime;
      int fps = 1000000 / duration;

      char buffer[40]; // Adjust the size as needed
      sprintf(buffer, "%d fps, %s", fps, hwsw);

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
