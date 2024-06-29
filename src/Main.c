#include <SDL2/SDL.h>
#include <SDL_pixels.h>
#include <bits/stdint-intn.h>
#include <bits/stdint-uintn.h>
#include <bits/time.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#include "ProToMath.h"
#include "TextureLoader.h"

#define WHITE_COLOR (0 << 24) | (255 << 16) | (255 << 8) | 255
#define GREY_COLOR (0 << 24) | (50 << 16) | (50 << 8) | 50
#define DARKER_GREY_COLOR (0 << 24) | (30 << 16) | (30 << 8) | 30
#define BLACK_COLOR (0 << 24) | (0 << 16) | (0 << 8) | 0
#define RED_COLOR (0 << 24) | (255 << 16) | (0 << 8) | 0
#define GREEN_COLOR (0 << 24) | (0 << 16) | (255 << 8) | 0
#define BLUE_COLOR (0 << 24) | (0 << 16) | (0 << 8) | 255
#define YELLOW_COLOR (0 << 24) | (255 << 16) | (0 << 8) | 255

typedef struct {
  float x, y;
} Vector2;

typedef struct {
  int x, y;
} Vector2i;

typedef struct {
  int8_t x, y;
} Vector2i8;

typedef struct {
  float moveDirRad;
  float speed;
  Vector2 pos;
  float rot;
  uint16_t fov;
} Player;

typedef struct {
  uint8_t r, g, b;
} RGB;

typedef struct {
  int width, height;
  uint32_t *pixelBuffer;
} WindowData;

// clang-format off
int32_t map[16][16] = {{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
                      {1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1},
                      {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
                      {1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
                      {1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
                      {1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
                      {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
                      {1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
                      {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
                      {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
                      {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
                      {1, 0, 0, 0, 0, 2, 2, 2, 2, 0, 0, 0, 0, 0, 0, 1},
                      {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
                      {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
                      {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}};


// int32_t map[1][1] = {1};
// clang-format on

long GetMicroTime() {
  // struct timespec currentTime;
  // clock_gettime(CLOCK_MONOTONIC, &currentTime);
  // printf("%ld\n", currentTime.tv_nsec);
  // return currentTime.tv_nsec;

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

void AddPixelToBuffer(WindowData wd, int x, int y, uint32_t color) {
  if (x >= 0 && x < wd.width && y >= 0 && y < wd.height) {
    wd.pixelBuffer[y * wd.width + x] = color;
  }
}

void AddCircle(WindowData wd, float radius, Vector2i circlePos, uint32_t color) {
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
  while (x > y) {
    y++;

    if (p <= 0) {
      p = p + 2 * y + 1;
    } else {
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

void PlotLineLow(WindowData wd, Vector2i from, Vector2i to, uint32_t color) {
  const int dx = to.x - from.x;
  int dy = to.y - from.y;

  int yi = 1;

  if (dy < 0) {
    yi = -1;
    dy = -dy;
  }

  int d = 2 * dy - dx;
  int y = from.y;

  for (int x = from.x; x < to.x; x++) {
    AddPixelToBuffer(wd, x, y, color);
    if (d > 0) {
      y = y + yi;
      d = d + (2 * (dy - dx));
    } else {
      d = d + 2 * dy;
    }
  }
}

void PlotLineHigh(WindowData wd, Vector2i from, Vector2i to, uint32_t color) {
  int dx = to.x - from.x;
  const int dy = to.y - from.y;

  int xi = 1;

  if (dx < 0) {
    xi = -1;
    dx = -dx;
  }

  int d = 2 * dx - dy;
  int x = from.x;

  for (int y = from.y; y < to.y; y++) {
    AddPixelToBuffer(wd, x, y, color);
    if (d > 0) {
      x = x + xi;
      d = d + (2 * (dx - dy));
    } else {
      d = d + 2 * dx;
    }
  }
}

void AddLine(WindowData wd, Vector2i from, Vector2i to, uint32_t color) {
  if (abs(to.y - from.y) < abs(to.x - from.x)) {
    if (from.x > to.x)
      PlotLineLow(wd, to, from, color);
    else
      PlotLineLow(wd, from, to, color);
  } else {
    if (from.y > to.y)
      PlotLineHigh(wd, to, from, color);
    else
      PlotLineHigh(wd, from, to, color);
  }

  AddPixelToBuffer(wd, from.x, from.y, color);
  AddPixelToBuffer(wd, to.x, to.y, color);
}

Vector2i CalculateLineEndpoint(Vector2i from, float length, float angle) {
  Vector2i arrowEndPoint;
  arrowEndPoint.x = from.x + cosf(angle) * length;
  arrowEndPoint.y = from.y + sinf(angle) * length;
  return arrowEndPoint;
}

void AddLineWithArrow(WindowData wd, Vector2i from, Vector2i to, float rot, uint32_t color) {
  AddLine(wd, from, to, color);

  float arrowHeadAngle = deg2rad(135);
  for (int i = 0; i < 2; i++) {
    Vector2i arrowheadEndPoint = CalculateLineEndpoint(to, 6.0f, rot - arrowHeadAngle);
    AddLine(wd, to, arrowheadEndPoint, color);
    arrowHeadAngle += M_PI_2;
  }
}

void AddLineInDirectionWithArrow(WindowData wd, Vector2i from, float length, float rot, uint32_t color) {
  Vector2i lineEndpoint = CalculateLineEndpoint(from, length, rot);
  AddLineWithArrow(wd, from, lineEndpoint, rot, color);
}

void AddLineInDirection(WindowData wd, Vector2i from, float length, float rot, uint32_t color) {
  Vector2i lineEndpoint = CalculateLineEndpoint(from, length, rot);
  AddLine(wd, from, lineEndpoint, color);
}

void FillWithNoise(WindowData wd) {
  for (int p = 0; p < wd.width * wd.height; p++) {
    uint32_t randomColor = random();
    wd.pixelBuffer[p] = randomColor;
  }
}

void CastRays(WindowData wd, Player player, uint32_t *wallTextures[]) {
  for (int ray = 0; ray < wd.width; ray++) {
    const float rayAngle = deg2rad((rad2deg(player.rot) - (player.fov / 2.0f)) + ((float)ray / wd.width) * player.fov);

    const float dx = cosf(rayAngle);
    const float dy = sinf(rayAngle);

    int mapX = (int)player.pos.x;
    int mapY = (int)player.pos.y;

    float sideDistX, sideDistY;

    const float deltaDistX = fabsf(1 / dx);
    const float deltaDistY = fabsf(1 / dy);
    float distance;

    int8_t stepX, stepY;

    float hitPointX, hitPointY;

    uint32_t *wallTexture;

    if (dx < 0) {
      stepX = -1;
      sideDistX = (player.pos.x - (float)mapX) * deltaDistX;
    } else {
      stepX = 1;
      sideDistX = (float)(mapX + 1.0f - player.pos.x) * deltaDistX;
    }
    if (dy < 0) {
      stepY = -1;
      sideDistY = (player.pos.y - (float)mapY) * deltaDistY;
    } else {
      stepY = 1;
      sideDistY = ((float)mapY + 1.0f - player.pos.y) * deltaDistY;
    }

    bool side;
    int attempt = 0;
    bool outside = false;
    while (attempt < 64) {
      attempt++;

      if (sideDistX < sideDistY) {
        sideDistX += deltaDistX;
        mapX += stepX;
        side = false;
      } else {
        sideDistY += deltaDistY;
        mapY += stepY;
        side = true;
      }

      if (mapX > 16 || mapX < 0 || mapY > 16 || mapY < 0) {
        outside = true;
        return;
      }

      if (map[mapX][mapY] == 1) {
        wallTexture = wallTextures[0];
        break;
      } else if (map[mapX][mapY] == 2) {
        wallTexture = wallTextures[1];
        break;
      }
    }

    if (!side) // if hit a horizontal wall
    {
      distance = (mapX - player.pos.x + (1.0f - stepX) / 2.0f) / dx;
      hitPointX = mapX + ((float)stepX / 2);
      hitPointY = player.pos.y + distance * dy;
    } else // if hit a vertical wall
    {
      distance = (mapY - player.pos.y + (1.0f - stepY) / 2.0f) / dy;
      hitPointX = player.pos.x + distance * dx;
      hitPointY = mapY + ((float)stepY / 2);
    }

    distance = distance * cosf(rayAngle - player.rot);

    int wallHeight = (int)(wd.height / distance * (float)(100.0f / player.fov));
    int middle = wd.height / 2;

    int startPos = middle - wallHeight / 2;
    if (startPos < 0)
      startPos = 0;

    int endPos = middle + wallHeight / 2;
    if (endPos >= wd.height) {
      endPos = wd.height - 1;
    }
    const float c = side ? hitPointX - floorf(hitPointX) : hitPointY - floorf(hitPointY);

    // offset is needed for walls that are very close to the player so they wont stick to the top of the screen
    // it stays 0 if wall height is smaller than the screen height
    float offset = 0;
    if (wallHeight > wd.height) {
      offset = (wallHeight - wd.height) / 2.0f;
    }

    float stepBetweenHorizontalSegments = 64.0f / (float)wallHeight;
    float horizontalSegment = offset * stepBetweenHorizontalSegments;

    float percentage = ((distance - 4.0f) / (16.0f - 4.0f));

    percentage = 1.0f - percentage;

    if (distance < 4.0f) {
      percentage = 1.0f;
    }
    if (distance > 16.0f) {
      percentage = 0.0f;
    }

    if (percentage < 0.25f)
      percentage = 0.25f;

    for (int pixel = startPos; pixel < endPos; pixel++) {
      const int verticalSegment = 64.0f * c;

      uint32_t color = wallTexture[(int)horizontalSegment * 64 + verticalSegment];
      horizontalSegment += stepBetweenHorizontalSegments;

      RGB rgb = UnpackRGB(color);

      rgb.r *= percentage;
      rgb.g *= percentage;
      rgb.b *= percentage;

      uint32_t reColor = PackRGB(rgb.r, rgb.g, rgb.b);

      // AddPixelToBuffer(wd, ray, pixel, reColor);
      int index = pixel * wd.width + ray;
      wd.pixelBuffer[index] = reColor;
    }
  }
}

void HardwareRenderer(WindowData wd, SDL_Renderer *renderer, SDL_Window *window, SDL_Texture *texture) {
  uint32_t *pixels;
  int pitch;

  // lock texture
  if (SDL_LockTexture(texture, NULL, (void **)&pixels, &pitch) != 0) {
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    printf("SDL_LockTexture Error: %s\n", SDL_GetError());
    SDL_Quit();
  }

  uint32_t *pixPtr = (uint32_t *)pixels;

  for (int p = 0; p < wd.width * wd.height; p++) {
    const int x = p % wd.width;
    const int y = p / wd.width;
    const int i = y * wd.width + x;

    pixPtr[i] = wd.pixelBuffer[i];
  }

  SDL_UnlockTexture(texture);
  SDL_RenderCopy(renderer, texture, NULL, NULL);
  SDL_RenderPresent(renderer);
}

void SoftwareRenderer(WindowData wd, SDL_Renderer *renderer) {
  for (int p = 0; p < wd.width * wd.height; p++) {
    const int x = p % wd.width;
    const int y = p / wd.width;
    const int i = y * wd.width + x;

    // if (wd.pixelBuffer[i] == 0)
    // {
    //   continue;
    // }

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

Vector2i StartingPositionForCentering(WindowData wd, int objectWidth, int objectHeight) {
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

void DrawMap(WindowData wd, Player player, int resScale) {
  const int mapWidth = 16;
  const int mapHeight = 16;

  Vector2i playerPosOnMap;
  playerPosOnMap.x = player.pos.x;
  playerPosOnMap.y = player.pos.y;

  for (int i = 0; i < mapWidth * mapHeight; i++) {
    int x = i % mapWidth;
    int y = i / mapWidth;

    // map squares
    if (map[x][y] == 1) {
      AddPixelToBuffer(wd, x, y, BLUE_COLOR);
    } else if (map[x][y] == 2) {
      AddPixelToBuffer(wd, x, y, WHITE_COLOR);
    }

    else {
      AddPixelToBuffer(wd, x, y, BLACK_COLOR);
    }
  }

  Vector2i centerPos;
  centerPos.x = wd.width / 2;
  centerPos.y = wd.height / 2;

  // draw player arrow in center
  AddLineInDirectionWithArrow(wd, playerPosOnMap, 12.0f, player.rot, RED_COLOR);

  Vector2i to;
  // to.x = raycastHitPoints[0];
  // to.y = raycastHitPoints[1];

  // printf("Hit sector, X: %d, Y: %d\n", raycastHitPoints[0], raycastHitPoints[1]);

  AddPixelToBuffer(wd, playerPosOnMap.x, playerPosOnMap.y, GREEN_COLOR);
  AddPixelToBuffer(wd, to.x, to.y - 1, YELLOW_COLOR);
  // AddLine(wd, playerPosOnMap, to, YELLOW_COLOR);

  // direction arrow for player
  if (player.speed != 0) {
    AddLineInDirectionWithArrow(wd, playerPosOnMap, 8.0f, player.rot + player.moveDirRad, GREEN_COLOR);
  }
}

int CalculateAverageFps(int executionTime) {
  const int FPS_HISTORY_SIZE = 64;

  static int fpsHistory[64];

  for (int i = FPS_HISTORY_SIZE; i >= 0; i--) {
    int nexti = i + 1;
    // printf("%d\n", i);
    if (nexti <= FPS_HISTORY_SIZE - 1) {
      fpsHistory[nexti] = fpsHistory[i];
    }
  }
  fpsHistory[0] = 1000000 / executionTime;

  int32_t sumFps = 0;
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
    hwsw[0] = 'H';
    // strcpy(hwsw, "HW");
  } else {
    hwsw[0] = 'S';
    // strcpy(hwsw, "SW");
  }
}
void ToggleMap(bool *mapEnabled) { *mapEnabled = !(*mapEnabled); }

int main() {
  int windowWidth = 2560;
  int windowHeight = 1440;

  uint32_t *wallTextures[16];
  wallTextures[0] = LoadTexture(0);
  wallTextures[1] = LoadTexture(1);

  // uint32_t *wall1 = LoadTexture(0);
  // uint32_t *wall2 = LoadTexture(1);

  float resScale = 0.2f;

  int width = windowWidth * resScale;
  int height = windowHeight * resScale;

  // initialize sdl
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    SDL_Log("SDL could not initialize! SDL_Error: %s", SDL_GetError());
    return EXIT_FAILURE;
  }

  // create window
  SDL_Window *window = SDL_CreateWindow("", SDL_WINDOWPOS_CENTERED, SDL_WINDOW_FULLSCREEN, windowWidth, windowHeight, SDL_WINDOW_SHOWN);
  if (window == NULL) {
    SDL_Log("Window could not be created! SDL_Error: %s", SDL_GetError());
    SDL_Quit();
    return EXIT_FAILURE;
  }

  // SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);

  // create renderer
  SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
  if (renderer == NULL) {
    SDL_Log("Renderer could not be created! SDL_Error: %s", SDL_GetError());
    SDL_DestroyWindow(window);
    SDL_Quit();
    return EXIT_FAILURE;
  }

  // set resolution inside the window
  SDL_RenderSetLogicalSize(renderer, width, height);

  // create the texture that will display content in the window
  SDL_Texture *texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGB888, SDL_TEXTUREACCESS_STREAMING, width, height);
  if (texture == NULL) {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    printf("SDL_CreateTexture Error: %s\n", SDL_GetError());
    SDL_Quit();
    return EXIT_FAILURE;
  }

  // create a 2D array that will store colors of each pixel
  uint32_t *pixelBuffer = malloc(width * height * sizeof(uint32_t));
  // uint32_t pixelBuffer[width * height];
  if (pixelBuffer == NULL) {
    perror("Failed to allocate memory for 2D pixel array");
    return EXIT_FAILURE;
  }

  // create an array to store ray cast hit coordinates
  // int32_t *raycastHitPoints = malloc(width * 2 * sizeof(int32_t));
  // if (raycastHitPoints == NULL) {
  //   perror("Failed to allocate memory for raycast hits array");
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
  player.rot = 0;
  player.pos.x = 2;
  player.pos.y = 4;
  player.fov = 90;

  float playerSpeedDefault = 4.0f;

  // key pressed values
  bool wKeyPressed = false;
  bool sKeyPressed = false;
  bool aKeyPressed = false;
  bool dKeyPressed = false;

  bool tabKeyPressed = false;

  bool nKeyPressed = false;
  bool hKeyPressed = false;

  // map
  bool mapEnabled = false;

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
  SDL_SetRelativeMouseMode(true);
  Vector2i mousePosition;

  // uint32_t test[1] = {RED_COLOR};

  // for (int i = 0; i < 2048; i++) {
  //   test[i] = GREEN_COLOR;
  // }

  SDL_Event event;
  bool running = true;
  while (running) {
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
          wKeyPressed = true;
        }
        if (event.key.keysym.sym == SDLK_s) {
          sKeyPressed = true;
        }
        if (event.key.keysym.sym == SDLK_a) {
          aKeyPressed = true;
        }
        if (event.key.keysym.sym == SDLK_d) {
          dKeyPressed = true;
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
          wKeyPressed = false;
        }
        if (event.key.keysym.sym == SDLK_s) {
          sKeyPressed = false;
        }
        if (event.key.keysym.sym == SDLK_a) {
          aKeyPressed = false;
        }
        if (event.key.keysym.sym == SDLK_d) {
          dKeyPressed = false;
        }
        break;
      case SDL_MOUSEMOTION:
        // player.rotation += (width / 2.0f - event.motion.x) / 128;
        player.rot += deg2rad(event.motion.xrel) / 8 / resScale;

        if (player.rot < -M_PI) {
          player.rot += 2 * M_PI;
        } else if (player.rot > M_PI) {
          player.rot -= 2 * M_PI;
        }

        // player.rot = -player.rot;

        // mousePosition.x = event.motion.x;
        // mousePosition.y = event.motion.y;

        // printf("%.6f\n", player.rot);

        break;
      }
    }

    int8_t forwards, sideways;
    sideways = dKeyPressed - aKeyPressed;
    forwards = wKeyPressed - sKeyPressed;

    if (sideways != 0 || forwards != 0) {
      player.speed = playerSpeedDefault;
    } else {
      player.speed = 0;
    }

    const float speedMultiplier = 0.0166f * player.speed * deltaTime;

    player.moveDirRad = atan2f(sideways, forwards);

    const int colX = (int)floorf(player.pos.x + cosf(player.rot + player.moveDirRad) / 2.0f);
    const int colY = (int)floorf(player.pos.y + sinf(player.rot + player.moveDirRad) / 2.0f);

    // printf("%d, %d\n", colX, colY);

    if (map[colX][colY] != 0) {

    } else {
      player.pos.x += cosf(player.rot + player.moveDirRad) * speedMultiplier;
      player.pos.y += sinf(player.rot + player.moveDirRad) * speedMultiplier;
    }

    // reset pixel buffer
    for (int p = 0; p < width * height; p++) {
      int x = p % width;
      int y = p / width;

      if (y > height / 2)
        pixelBuffer[p] = GREY_COLOR;
      else
        pixelBuffer[p] = DARKER_GREY_COLOR;
    }

    if (noiseEnabled) {
      FillWithNoise(wd);
    }

    if (mapEnabled) {
      DrawMap(wd, player, resScale);
    } else {
      CastRays(wd, player, wallTextures);
    }

    // render
    if (hardwareAcceleration) {
      SDL_UpdateTexture(texture, NULL, wd.pixelBuffer, wd.width * 4);
      SDL_RenderCopy(renderer, texture, NULL, NULL);
      SDL_RenderPresent(renderer);
      // HardwareRenderer(wd, renderer, window, texture);
    } else {
      SoftwareRenderer(wd, renderer);
    }

    // printf("execution time: %d\n", executionTime);
    if (limitSpeed) {
      int executionTime = GetMicroTime() - startTime;
      int timeToSleep = 16666 - executionTime;

      if (timeToSleep > 0) {
        usleep(timeToSleep);
      }
    }

    int32_t elapsedTime = GetMicroTime() - currentTime;
    int executionTimeWithSleep = GetMicroTime() - startTime;

    int avgFps = CalculateAverageFps(executionTimeWithSleep);
    deltaTime = (GetMicroTime() - startTime) * 60.0f / 1000000.0f;

    // printf("%.6f\n", deltaTime);
    // long frameTime = GetMicroTime() - startTime;
    // float gameSpeed = ((1.0f / frameTime / 60.0f) * 1000000.0f) * deltaTime;

    // Break the loop after 1 second (1,000,000 microseconds)
    if (elapsedTime >= 1000000) {
      // int duration = GetMicroTime() - startTime;
      // printf("elapsed time: %d\n", elapsedTime);

      char buffer[15];
      sprintf(buffer, "%dx%d - %d fps - %s", width, height, avgFps, hwsw);

      SDL_SetWindowTitle(window, buffer);
      currentTime = GetMicroTime();
    }
  }

  // Free the allocated memory
  // free(pixelBuffer);

  // Clean up
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();

  return 0;
}
