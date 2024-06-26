#include <SDL2/SDL.h>
#include <bits/stdint-intn.h>
#include <bits/stdint-uintn.h>
#include <bits/time.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>

#include "ProToMath.h"

#define WHITE_COLOR (0 << 24) | (255 << 16) | (255 << 8) | 255
#define BLACK_COLOR (0 << 24) | (0 << 16) | (0 << 8) | 0
#define RED_COLOR (0 << 24) | (255 << 16) | (0 << 8) | 0
#define GREEN_COLOR (0 << 24) | (0 << 16) | (255 << 8) | 0
#define BLUE_COLOR (0 << 24) | (0 << 16) | (0 << 8) | 255

typedef struct
{
  float x, y;
} Vector2;

typedef struct
{
  int x, y;
} Vector2i;

typedef struct
{
  int8_t x, y;
} Vector2i8;

typedef struct
{
  float moveDirRad;
  float speed;
  Vector2 pos;
  float rot;
  uint8_t fov;
} Player;

typedef struct
{
  uint8_t r, g, b;
} RGB;

typedef struct
{
  int width, height;
  uint32_t *pixelBuffer;
} WindowData;

int8_t map[8][8] = {{1, 1, 1, 1, 1, 1, 1, 1},
                    {1, 0, 0, 0, 0, 0, 0, 1},
                    {1, 0, 0, 0, 0, 1, 0, 1},
                    {1, 0, 0, 0, 0, 0, 0, 1},
                    {1, 0, 0, 0, 0, 0, 0, 1},
                    {1, 0, 0, 0, 0, 0, 0, 1},
                    {1, 1, 0, 0, 0, 0, 0, 1},
                    {1, 1, 1, 1, 1, 1, 1, 1}};

long GetMicroTime()
{
  // struct timespec currentTime;
  // clock_gettime(CLOCK_MONOTONIC, &currentTime);
  // printf("%ld\n", currentTime.tv_nsec);
  // return currentTime.tv_nsec;

  struct timeval currentTime;
  gettimeofday(&currentTime, NULL);
  return currentTime.tv_sec * (int)1e6 + currentTime.tv_usec;
}

uint32_t PackRGB(uint8_t r, uint8_t g, uint8_t b)
{
  uint32_t argb = 0;
  argb |= 0 << 24;
  argb |= r << 16;
  argb |= g << 8;
  argb |= b;

  return argb;
}

RGB UnpackRGB(uint32_t packedRgb)
{
  const uint8_t r = (packedRgb >> 16) & 0xFF;
  const uint8_t g = (packedRgb >> 8) & 0xFF;
  const uint8_t b = (packedRgb) & 0xFF;
  RGB rgb = {r, g, b};
  return rgb;
}

void AddPixelToBuffer(WindowData wd, int x, int y, uint32_t color)
{
  if (x >= 0 && x < wd.width && y >= 0 && y < wd.height)
  {
    wd.pixelBuffer[y * wd.width + x] = color;
  }
}

void AddCircle(WindowData wd, float radius, Vector2i circlePos, uint32_t color)
{
  // int t1 = radius / 16;
  int x = radius;
  int y = 0;

  AddPixelToBuffer(wd, circlePos.x + x, circlePos.y + y, color);
  AddPixelToBuffer(wd, circlePos.x - x, circlePos.y + y, color);
  // AddPixelToBuffer(wd, circlePos.x + x, circlePos.y - y, color);
  // AddPixelToBuffer(wd, circlePos.x + x, circlePos.y + y, color);

  // if (radius > 0)
  // {
  //   AddPixelToBuffer(wd, x + circlePos.x, -y + circlePos.y, color);
  //   AddPixelToBuffer(wd, y + circlePos.x, x + circlePos.y, color);
  //   AddPixelToBuffer(wd, y + circlePos.x, x + circlePos.y, color);
  //   AddPixelToBuffer(wd, -y + circlePos.x, x + circlePos.y, color);
  // }

  int p = 1 - radius;
  while (x > y)
  {
    y++;

    if (p <= 0)
    {
      p = p + 2 * y + 1;
    }
    else
    {
      x--;
      p = p + 2 * y - 2 * x + 1;
    }

    if (x < y)
      break;

    AddPixelToBuffer(wd, circlePos.x + x, circlePos.y + y, color);
    AddPixelToBuffer(wd, circlePos.x + -x, circlePos.y + y, color);
    AddPixelToBuffer(wd, circlePos.x + x, circlePos.y + -y, color);
    AddPixelToBuffer(wd, circlePos.x + -x, circlePos.y + -y, color);

    // if (x != y)
    // {
    AddPixelToBuffer(wd, circlePos.x + y, circlePos.y + x, color);
    AddPixelToBuffer(wd, circlePos.x + -y, circlePos.y + x, color);
    AddPixelToBuffer(wd, circlePos.x + y, circlePos.y + -x, color);
    AddPixelToBuffer(wd, circlePos.x + -y, circlePos.y + -x, color);
    // }
  }
  // for (float i = 0; i < 360; i++)
  // {
  //   const float angle = (float)i;

  //   const float yf = radius * sinf(deg2rad(i));
  //   const float xf = sqrtf(sqr(radius) - sqr(yf));

  //   int x, y;

  //   if (angle >= 0.0f && angle < 90.0f)
  //   {
  //     x = circlePos.x + round(xf);
  //     y = circlePos.y + roundf(yf);
  //   }
  //   else if (angle >= 90.0f && angle < 180.0f)
  //   {
  //     x = circlePos.x + roundf(xf);
  //     y = circlePos.y - roundf(yf);
  //   }
  //   else if (angle >= 180.0f && angle < 270.f)
  //   {
  //     x = circlePos.x - roundf(xf);
  //     y = circlePos.y - roundf(yf);
  //   }
  //   else if (angle >= 270.0f && angle <= 360.0f)
  //   {
  //     x = circlePos.x - roundf(xf);
  //     y = circlePos.y + roundf(yf);
  //   }
  //   else
  //   {
  //     printf("wrong angle\n");
  //     continue;
  //   }
  //   AddPixelToBuffer(wd, x, y, WHITE_COLOR);
  // }
  // // dot in center
  // AddPixelToBuffer(wd, circlePos.x, circlePos.y, WHITE_COLOR);
}

void PlotLineLow(WindowData wd, Vector2i from, Vector2i to, uint32_t color)
{
  const int dx = to.x - from.x;
  int dy = to.y - from.y;

  int yi = 1;

  if (dy < 0)
  {
    yi = -1;
    dy = -dy;
  }

  int d = 2 * dy - dx;
  int y = from.y;

  for (int x = from.x; x < to.x; x++)
  {
    AddPixelToBuffer(wd, x, y, color);
    if (d > 0)
    {
      y = y + yi;
      d = d + (2 * (dy - dx));
    }
    else
    {
      d = d + 2 * dy;
    }
  }
}

void PlotLineHigh(WindowData wd, Vector2i from, Vector2i to, uint32_t color)
{
  int dx = to.x - from.x;
  const int dy = to.y - from.y;

  int xi = 1;

  if (dx < 0)
  {
    xi = -1;
    dx = -dx;
  }

  int d = 2 * dx - dy;
  int x = from.x;

  for (int y = from.y; y < to.y; y++)
  {
    AddPixelToBuffer(wd, x, y, color);
    if (d > 0)
    {
      x = x + xi;
      d = d + (2 * (dx - dy));
    }
    else
    {
      d = d + 2 * dx;
    }
  }
}

void AddLine(WindowData wd, Vector2i from, Vector2i to, uint32_t color)
{
  if (abs(to.y - from.y) < abs(to.x - from.x))
  {
    if (from.x > to.x)
      PlotLineLow(wd, to, from, color);
    else
      PlotLineLow(wd, from, to, color);
  }
  else
  {
    if (from.y > to.y)
      PlotLineHigh(wd, to, from, color);
    else
      PlotLineHigh(wd, from, to, color);
  }

  AddPixelToBuffer(wd, from.x, from.y, color);
  AddPixelToBuffer(wd, to.x, to.y, color);
}

Vector2i CalculateLineEndpoint(Vector2i endPoint, float length, float angle)
{
  Vector2i arrowEndPoint;
  arrowEndPoint.x = endPoint.x + cosf(angle) * length;
  arrowEndPoint.y = endPoint.y + sinf(angle) * length;
  return arrowEndPoint;
}

void AddLineWithArrow(WindowData wd, Vector2i from, Vector2i to, float rot, uint32_t color)
{
  AddLine(wd, from, to, color);

  float arrowHeadAngle = deg2rad(135);
  for (int i = 0; i < 2; i++)
  {
    Vector2i arrowheadEndPoint = CalculateLineEndpoint(to, 6.0, rot - arrowHeadAngle);
    AddLine(wd, to, arrowheadEndPoint, color);
    arrowHeadAngle += M_PI_2;
  }
}

void AddLineInDirectionWithArrow(WindowData wd, Vector2i from, float length, float rot, uint32_t color)
{
  Vector2i lineEndpoint = CalculateLineEndpoint(from, length, rot);
  AddLineWithArrow(wd, from, lineEndpoint, rot, color);
}

void AddLineInDirection(WindowData wd, Vector2i from, float length, float rot, uint32_t color)
{
  Vector2i lineEndpoint = CalculateLineEndpoint(from, length, rot);
  AddLine(wd, from, lineEndpoint, color);
}

void FillWithNoise(WindowData wd)
{
  for (int p = 0; p < wd.width * wd.height; p++)
  {
    uint32_t randomColor = random();
    wd.pixelBuffer[p] = randomColor;
  }
}

void RayHit(WindowData wd, int column)
{
  const int wallHeight = RandomMax(150);

  const int middle = wd.height / 2;
  const int startPos = middle - wallHeight / 2;
  const int endPos = startPos + wallHeight;

  for (int pixel = startPos; pixel < endPos; pixel++)
  {
    AddPixelToBuffer(wd, column, pixel, WHITE_COLOR);
  }
}

void CastRays(WindowData wd)
{
  for (int column = 0; column < wd.width; column++)
  {
    RayHit(wd, column);
  }
}

void DrawWalls(WindowData wd)
{
  for (int l = 0; l < wd.width; l++)
  {
    const int index = wd.height / 2 * wd.width + l;
    wd.pixelBuffer[index] = RED_COLOR;
  }
}

void SoftwareRenderer(WindowData wd, SDL_Renderer *renderer)
{
  for (int p = 0; p < wd.width * wd.height; p++)
  {
    const int x = p % wd.width;
    const int y = p / wd.width;
    const int i = y * wd.width + x;

    if (wd.pixelBuffer[i] == 0)
    {
      continue;
    }

    const RGB rgb = UnpackRGB(wd.pixelBuffer[i]);
    SDL_SetRenderDrawColor(renderer, rgb.r, rgb.g, rgb.b, 255);
    SDL_RenderDrawPoint(renderer, x, y);
  }
  SDL_RenderPresent(renderer);
}

// void AddPlayer(WindowData wd, Player player)
// {
//   Vector2i playerPosMap = {roundf(player.pos.x), -roundf(player.pos.y)};

//   float radius = 128.0f;
//   AddCircle(wd, radius, playerPosMap, WHITE_COLOR);
// }

Vector2i StartingPositionForCentering(WindowData wd, int objectWidth, int objectHeight)
{
  Vector2i pos;
  pos.x = wd.width / 2 - objectWidth / 2;
  pos.y = wd.height / 2 - objectHeight / 2;
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

void DrawMap(WindowData wd, Player player, int resScale)
{
  const int upscaleMultiplier = 64 / resScale;
  const int mapSize = 8;

  Vector2i pos = StartingPositionForCentering(wd, upscaleMultiplier * mapSize, upscaleMultiplier * mapSize);

  pos.x = pos.x - roundf(player.pos.x) / resScale;
  pos.y = pos.y - -roundf(player.pos.y) / resScale;

  // int midX = width / 2;
  // int midY = height / 2;

  for (int y = 0; y < upscaleMultiplier; y++)
  {
    for (int x = 0; x < upscaleMultiplier; x++)
    {
      for (int mapY = 0; mapY < mapSize; mapY++)
      {
        for (int mapX = 0; mapX < mapSize; mapX++)
        {
          int screenY = mapY * upscaleMultiplier + y + pos.y;
          int screenX = mapX * upscaleMultiplier + x + pos.x;

          // int index = screenY * width + screenX;

          // map squares
          if (map[mapX][mapY] == 1)
          {
            AddPixelToBuffer(wd, screenX, screenY, BLUE_COLOR);
          }
          else
          {
            AddPixelToBuffer(wd, screenX, screenY, BLACK_COLOR);
          }

          // // outlines
          // if (x == 0 || x == upscaleMultiplier - 1 || y == 0 ||
          //     y == upscaleMultiplier - 1) {
          //   AddPixelToBuffer(width, height, pixelBuffer, screenX, screenY,
          //                    WHITE_COLOR);
          // }
        }
      }
    }
  }

  // const int playerSize = 8 / resScale;
  // Vector2i centerDotPos = StartingPositionForCentering(wd, playerSize, playerSize);

  Vector2i centerPos;
  centerPos.x = wd.width / 2;
  centerPos.y = wd.height / 2;

  // draw rays from player
  const float fovInRad = deg2rad(player.fov);
  const int rayCount = wd.width;

  const float startAngle = player.rot - fovInRad / 2;
  // float endAngle = player.rot + player.fov / 2;

  float currentAngle = startAngle;
  const float oneDegree = fovInRad / rayCount;

  for (int r = 0; r < rayCount; r++)
  {
    AddLineInDirection(wd, centerPos, 512, currentAngle, WHITE_COLOR);
    currentAngle += oneDegree;
    // printf("%f\n", currentAngle);
  }

  // draw player arrow in center
  AddLineInDirectionWithArrow(wd, centerPos, 12.0, player.rot, RED_COLOR);

  // direction arrow for player
  if (player.speed != 0)
  {
    AddLineInDirectionWithArrow(wd, centerPos, 8.0, player.rot + player.moveDirRad, GREEN_COLOR);
  }
}

int CalculateAverageFps(int executionTime)
{
  const int FPS_HISTORY_SIZE = 64;

  static int fpsHistory[64];

  for (int i = FPS_HISTORY_SIZE; i >= 0; i--)
  {
    int nexti = i + 1;
    // printf("%d\n", i);
    if (nexti <= FPS_HISTORY_SIZE - 1)
    {
      fpsHistory[nexti] = fpsHistory[i];
    }
  }
  fpsHistory[0] = 1000000 / executionTime;

  int32_t sumFps = 0;
  for (int i = 0; i < FPS_HISTORY_SIZE; i++)
  {
    sumFps += fpsHistory[i];
    // printf("%d: %d\n", i, fpsHistory[i]);
  }
  const int avgFps = sumFps / FPS_HISTORY_SIZE;
  return avgFps;
}

void ToggleHardwareAcceleration(bool *hardwareAcceleration, char *hwsw)
{
  *hardwareAcceleration = !(*hardwareAcceleration);
  if (*hardwareAcceleration)
  {
    hwsw[0] = 'H';
    // strcpy(hwsw, "HW");
  }
  else
  {
    hwsw[0] = 'S';
    // strcpy(hwsw, "SW");
  }
}
void ToggleMap(bool *mapEnabled) { *mapEnabled = !(*mapEnabled); }

int main()
{
  int windowWidth = 1920;
  int windowHeight = 1080;

  // int centerX = 1920 / 2;
  // int centerY = 1080 / 2;

  int resScale = 2;

  int width = windowWidth / resScale;
  int height = windowHeight / resScale;

  // initialize sdl
  if (SDL_Init(SDL_INIT_VIDEO) < 0)
  {
    SDL_Log("SDL could not initialize! SDL_Error: %s", SDL_GetError());
    return EXIT_FAILURE;
  }

  // create window
  SDL_Window *window = SDL_CreateWindow("", SDL_WINDOWPOS_CENTERED, SDL_WINDOW_FULLSCREEN, windowWidth, windowHeight, SDL_WINDOW_SHOWN);
  if (window == NULL)
  {
    SDL_Log("Window could not be created! SDL_Error: %s", SDL_GetError());
    SDL_Quit();
    return EXIT_FAILURE;
  }

  // SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);

  // create renderer
  SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
  if (renderer == NULL)
  {
    SDL_Log("Renderer could not be created! SDL_Error: %s", SDL_GetError());
    SDL_DestroyWindow(window);
    SDL_Quit();
    return EXIT_FAILURE;
  }

  // set resolution inside the window
  SDL_RenderSetLogicalSize(renderer, width, height);

  // create the texture that will display content in the window
  SDL_Texture *texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGB888, SDL_TEXTUREACCESS_STREAMING, width, height);
  if (texture == NULL)
  {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    printf("SDL_CreateTexture Error: %s\n", SDL_GetError());
    SDL_Quit();
    return EXIT_FAILURE;
  }

  // create a 2D array that will store colors of each pixel
  uint32_t *pixelBuffer = malloc(width * height * sizeof(uint32_t));
  // if (pixelBuffer == NULL)
  // {
  //   perror("Failed to allocate memory");
  //   return EXIT_FAILURE;
  // }

  WindowData wd;
  wd.pixelBuffer = pixelBuffer;
  wd.width = width;
  wd.height = height;

  // RNG
  // srand(time(0));

  // player values
  Player player;
  player.rot = 0.0;
  player.pos.x = 50.0;
  player.pos.y = -50.0;
  player.fov = 90;

  float playerSpeedDefault = 256.0f;

  // key pressed values
  bool wKeyPressed = false;
  bool sKeyPressed = false;
  bool aKeyPressed = false;
  bool dKeyPressed = false;

  bool tabKeyPressed = false;

  bool nKeyPressed = false;
  bool hKeyPressed = false;

  // map
  bool mapEnabled = true;

  // extra debug stuff
  bool limitSpeed = false;
  bool checkerBoardRendering = false;
  bool hardwareAcceleration = true;
  bool noiseEnabled = false;

  // needed for calculations inside the loop
  float deltaTime = 1.0f;
  long currentTime = GetMicroTime();
  // printf("%ld\n", currentTime);

  char hwsw[] = "HW";

  // SDL_ShowCursor(true);
  // SDL_SetRelativeMouseMode(true);
  Vector2i mousePosition;

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
        if (event.key.keysym.sym == SDLK_w)
        {
          wKeyPressed = true;
        }
        if (event.key.keysym.sym == SDLK_s)
        {
          sKeyPressed = true;
        }
        if (event.key.keysym.sym == SDLK_a)
        {
          aKeyPressed = true;
        }
        if (event.key.keysym.sym == SDLK_d)
        {
          dKeyPressed = true;
        }
        if (event.key.keysym.sym == SDLK_TAB)
        {
          ToggleMap(&mapEnabled);
        }
        if (event.key.keysym.sym == SDLK_n)
        {
          noiseEnabled = !noiseEnabled;
        }
        if (event.key.keysym.sym == SDLK_h)
        {
          ToggleHardwareAcceleration(&hardwareAcceleration, hwsw);
        }
        break;
      case SDL_KEYUP:
        if (event.key.keysym.sym == SDLK_w)
        {
          wKeyPressed = false;
        }
        if (event.key.keysym.sym == SDLK_s)
        {
          sKeyPressed = false;
        }
        if (event.key.keysym.sym == SDLK_a)
        {
          aKeyPressed = false;
        }
        if (event.key.keysym.sym == SDLK_d)
        {
          dKeyPressed = false;
        }
        break;
      case SDL_MOUSEMOTION:
        // player.rotation += (width / 2.0 - event.motion.x) / 128;
        player.rot += deg2rad(event.motion.xrel);

        if (player.rot < -M_PI)
        {
          player.rot += 2 * M_PI;
        }
        else if (player.rot > M_PI)
        {
          player.rot -= 2 * M_PI;
        }

        // mousePosition.x = event.motion.x;
        // mousePosition.y = event.motion.y;

        // printf("%.6f\n", player.rot);

        break;
      }
    }

    int8_t forwards, sideways;
    sideways = dKeyPressed - aKeyPressed;
    forwards = wKeyPressed - sKeyPressed;

    if (sideways != 0 || forwards != 0)
    {
      player.speed = playerSpeedDefault;
    }
    else
    {
      player.speed = 0;
    }

    float speedMultiplier = 0.0166f * player.speed * deltaTime;
    // player.pos.x += speedMultiplier * sideways;
    // player.pos.y += speedMultiplier * forwards;

    // gets the movement direction angle in radian

    player.moveDirRad = atan2f(sideways, forwards);

    player.pos.x += cosf(player.rot + player.moveDirRad) * speedMultiplier;
    player.pos.y -= sinf(player.rot + player.moveDirRad) * speedMultiplier;

    // static Vector2 previousPos;
    // previousPos.x = player.pos.x;
    // previousPos.y = player.pos.y;

    // reset pixel buffer
    if (hardwareAcceleration)
    {

      for (int p = 0; p < wd.width * wd.height; p++)
      {
        const int x = p % wd.width;
        const int y = p / wd.width;
        const int i = y * wd.width + x;

        wd.pixelBuffer[i] = BLACK_COLOR;
      }
    }
    else
    {
      SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
      SDL_RenderClear(renderer);
    }

    if (noiseEnabled)
    {
      FillWithNoise(wd);
    }

    if (mapEnabled)
    {
      DrawMap(wd, player, resScale);
    }
    else
    {
      CastRays(wd);
    }

    // render
    if (hardwareAcceleration)
    {
      SDL_UpdateTexture(texture, NULL, wd.pixelBuffer, wd.width * 4);
      SDL_RenderCopy(renderer, texture, NULL, NULL);
      SDL_RenderPresent(renderer);
    }
    else
    {
      SoftwareRenderer(wd, renderer);
    }

    // printf("execution time: %d\n", executionTime);
    if (limitSpeed)
    {
      int executionTime = GetMicroTime() - startTime;
      int timeToSleep = 16666 - executionTime;

      if (timeToSleep > 0)
      {
        usleep(timeToSleep);
      }
    }

    int32_t elapsedTime = GetMicroTime() - currentTime;
    int executionTimeWithSleep = GetMicroTime() - startTime;

    int avgFps = CalculateAverageFps(executionTimeWithSleep);
    deltaTime = (GetMicroTime() - startTime) * 60.0 / 1000000.0;

    // printf("%.6f\n", deltaTime);
    // long frameTime = GetMicroTime() - startTime;
    // float gameSpeed = ((1.0f / frameTime / 60.0f) * 1000000.0) * deltaTime;

    // Break the loop after 1 second (1,000,000 microseconds)
    if (elapsedTime >= 1000000)
    {
      // int duration = GetMicroTime() - startTime;
      // printf("elapsed time: %d\n", elapsedTime);

      char buffer[16];
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
