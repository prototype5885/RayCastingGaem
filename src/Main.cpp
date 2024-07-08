#include <SDL.h>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <ostream>
#include <string>
#include <thread>

#include "ProToMath.h"
#include "TextureLoader.h"

using std::cout;
using std::endl;
using std::string;
using std::to_string;

#define WHITE_COLOR ((0 << 24) | (255 << 16) | (255 << 8) | 255)
#define GREY_COLOR ((0 << 24) | (50 << 16) | (50 << 8) | 50)
#define DARKER_GREY_COLOR ((0 << 24) | (30 << 16) | (30 << 8) | 30)
#define BLACK_COLOR ((0 << 24) | (0 << 16) | (0 << 8) | 0)
#define RED_COLOR ((0 << 24) | (255 << 16) | (0 << 8) | 0)
#define GREEN_COLOR ((0 << 24) | (0 << 16) | (255 << 8) | 0)
#define BLUE_COLOR ((0 << 24) | (0 << 16) | (0 << 8) | 255)
#define YELLOW_COLOR ((0 << 24) | (255 << 16) | (255 << 8) | 0)

typedef struct {
  float x, y;
} Vector2;

typedef struct {
  int x, y;
} Vector2i;

typedef struct {
  int8_t x, y;
} Vector2i8;

// typedef struct {
//   float moveDirRad;
//   float speed;
//   Vector2 pos;
//   float rot;
//   float fov;
// } Player;

class Player {
public:
  float moveDirRad = 0.0f;
  float speed = 0.0f;
  Vector2 pos{0.0f, 0.0f};
  // float x = 0.0f;
  // float y = 0.0f;
  float rotRad = 0.0f;
  float rotDeg = 0.0f;
  float fov = 90.0f;

  // void ConvertRadToDeg() {
  //   rotDeg *= 57.29578;
  // }
};

// typedef struct {
//   uint8_t r, g, b;
// } RGB;

class RGB {
private:
  uint8_t r, g, b;

public:
  RGB(uint32_t rgb) {
    r = (rgb >> 16) & 0xFF;
    g = (rgb >> 8) & 0xFF;
    b = (rgb) & 0xFF;
  }

  uint32_t ReturnRGB() {
    uint32_t rgb = 0;
    rgb |= 0 << 24;
    rgb |= r << 16;
    rgb |= g << 8;
    rgb |= b;

    return rgb;
  }

  void Multiply(float multiplier) {
    r *= multiplier;
    g *= multiplier;
    b *= multiplier;
  }

  void Display() const { cout << "R: " << static_cast<int>(r) << ", G: " << static_cast<int>(g) << ", B: " << static_cast<int>(b) << std::endl; }
};

typedef struct {
  int width, height, size;
  uint32_t *pixels;
} DisplayData;

long GetMicroTime() {
  auto const now = std::chrono::steady_clock::now();

  auto const duration = now.time_since_epoch();
  return std::chrono::duration_cast<std::chrono::microseconds>(duration).count();
}

void AddPixelToBuffer(DisplayData *dd, int x, int y, uint32_t color) {
  int i = y * dd->width + x;
  if (i < dd->size && i > 0) {
    // cout << i << endl;
    dd->pixels[i] = color;
  }
}

void AddCircle(DisplayData *dd, float const radius, Vector2i const circlePos, uint32_t const color) {
  // int t1 = radius / 16;
  int x = static_cast<int>(radius);
  int y = 0;

  AddPixelToBuffer(dd, circlePos.x + x, circlePos.y + y, color);
  AddPixelToBuffer(dd, circlePos.x - x, circlePos.y + y, color);
  // AddPixelToBuffer(wd, circlex + x, circley - y, color);
  // AddPixelToBuffer(wd, circlex + x, circley + y, color);

  // if (radius > 0)
  // {
  //   AddPixelToBuffer(wd, x + circlex, -y + circley, color);
  //   AddPixelToBuffer(wd, y + circlex, x + circley, color);
  //   AddPixelToBuffer(wd, y + circlex, x + circley, color);
  //   AddPixelToBuffer(wd, -y + circlex, x + circley, color);
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

    // AddPixelToBuffer(wd, circlex + x, circley + y, color);
    // AddPixelToBuffer(wd, circlex + -x, circley + y, color);
    // AddPixelToBuffer(wd, circlex + x, circley + -y, color);
    // AddPixelToBuffer(wd, circlex + -x, circley + -y, color);

    // // if (x != y)
    // // {
    // AddPixelToBuffer(wd, circlex + y, circley + x, color);
    // AddPixelToBuffer(wd, circlex + -y, circley + x, color);
    // AddPixelToBuffer(wd, circlex + y, circley + -x, color);
    // AddPixelToBuffer(wd, circlex + -y, circley + -x, color);
    // // }
  }
}

void PlotLineLow(DisplayData *dd, Vector2i from, Vector2i to, uint32_t color) {
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
    AddPixelToBuffer(dd, x, y, color);
    if (d > 0) {
      y = y + yi;
      d = d + (2 * (dy - dx));
    } else {
      d = d + 2 * dy;
    }
  }
}

void PlotLineHigh(DisplayData *dd, Vector2i from, Vector2i to, uint32_t color) {
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
    AddPixelToBuffer(dd, x, y, color);
    if (d > 0) {
      x = x + xi;
      d = d + (2 * (dx - dy));
    } else {
      d = d + 2 * dx;
    }
  }
}

void AddLine(DisplayData *dd, Vector2i const from, Vector2i const to, uint32_t const color) {
  if (abs(to.y - from.y) < abs(to.x - from.x)) {
    if (from.x > to.x)
      PlotLineLow(dd, to, from, color);
    else
      PlotLineLow(dd, from, to, color);
  } else {
    if (from.y > to.y)
      PlotLineHigh(dd, to, from, color);
    else
      PlotLineHigh(dd, from, to, color);
  }

  AddPixelToBuffer(dd, from.x, from.y, color);
  AddPixelToBuffer(dd, to.x, to.y, color);
}

Vector2i CalculateLineEndpoint(Vector2i const from, float const length, float const angle) {
  Vector2i arrowEndPoint;
  arrowEndPoint.x = from.x + cosf(angle) * length;
  arrowEndPoint.y = from.y + sinf(angle) * length;
  return arrowEndPoint;
}

void AddLineWithArrow(DisplayData *dd, Vector2i const from, Vector2i const to, float const rot, uint32_t const color) {
  AddLine(dd, from, to, color);

  float arrowHeadAngle = deg2rad(135);
  for (int i = 0; i < 2; i++) {
    Vector2i arrowheadEndPoint = CalculateLineEndpoint(to, 6.0f, rot - arrowHeadAngle);
    AddLine(dd, to, arrowheadEndPoint, color);
    arrowHeadAngle += M_PI_2;
  }
}

void AddLineInDirectionWithArrow(DisplayData *dd, Vector2i const from, float const length, float const rot, uint32_t const color) {
  const Vector2i lineEndpoint = CalculateLineEndpoint(from, length, rot);
  AddLineWithArrow(dd, from, lineEndpoint, rot, color);
}

void AddLineInDirection(DisplayData *dd, Vector2i const from, float const length, float const rot, uint32_t const color) {
  const Vector2i lineEndpoint = CalculateLineEndpoint(from, length, rot);
  AddLine(dd, from, lineEndpoint, color);
}

void CastRays(DisplayData const *dd, Player const *player, int8_t const *map, uint32_t const *tileMap) {
  for (int ray = 0; ray < dd->width; ray++) {
    const float aspectRatio = static_cast<float>(dd->width) / static_cast<float>(dd->height);
    float rayAngle = player->rotRad - (aspectRatio / 2.0f);        // start angle of leftmost ray relative to player rotation
    rayAngle += (deg2rad(ray) / deg2rad(dd->width)) * aspectRatio; // then increment each ray in radian by this amount to the right

    const float dx = cosf(rayAngle);
    const float dy = sinf(rayAngle);

    int mapX = player->pos.x;
    int mapY = player->pos.y;

    float sideDistX, sideDistY;

    const float deltaDistX = fabsf(1.0f / dx);
    const float deltaDistY = fabsf(1.0f / dy);

    float distance;

    int stepX, stepY;

    float hitPointX, hitPointY;

    int tOffset = 0; // texture offset

    if (dx < 0.0f) {
      stepX = -1;
      sideDistX = (player->pos.x - (float)mapX) * deltaDistX;
    } else {
      stepX = 1;
      sideDistX = ((float)mapX + 1.0f - player->pos.x) * deltaDistX;
    }
    if (dy < 0.0f) {
      stepY = -1;
      sideDistY = (player->pos.y - (float)mapY) * deltaDistY;
    } else {
      stepY = 1;
      sideDistY = ((float)mapY + 1.0f - player->pos.y) * deltaDistY;
    }

    bool side;
    int attempt = 0;
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

      const int i = mapY * 16 + mapX;

      if (i > 255) {
        return;
      }

      int wallType = map[i];
      if (wallType == 1) {
        tOffset = 0 * 4096;
        break;
      } else if (wallType == 2) {
        tOffset = 1 * 4096;
        break;
      }
    }

    if (!side) { // if hit a horizontal wall
      distance = ((float)mapX - player->pos.x + (1.0f - (float)stepX) / 2.0f) / dx;
      hitPointX = (float)mapX + ((float)stepX / 2.0f);
      hitPointY = player->pos.y + distance * dy;
    } else { // if hit a vertical wall
      distance = (mapY - player->pos.y + (1.0f - stepY) / 2.0f) / dy;
      hitPointX = player->pos.x + distance * dx;
      hitPointY = mapY + ((float)stepY / 2.0f);
    }

    distance = distance * cosf(rayAngle - player->rotRad); // fisheye fix

    // const int wallHeight = height / distance * (100.0f / player.fov); // this is how tall the wall will be based on ray distance
    const int wallHeight = dd->height / distance; // this is how tall the wall will be based on ray distance
    const int middle = dd->height / 2;            // middle of the screen

    int startPos = middle - wallHeight / 2; // wall starts at this height
    if (startPos < 0)                       // prevent it from starting from above the screen
      startPos = 0;

    int endPos = middle + wallHeight / 2; // wall ends here
    if (endPos > dd->height)              // prevent it from starting from below the screen
      endPos = dd->height;

    float percentage = 1.0 - (distance - 4.0f) / (16.0f - 4.0f);

    if (distance < 4.0f) {
      percentage = 1.0f;
    } else if (distance > 16.0f) {
      percentage = 0.0f;
    }

    if (percentage < 0.25f)
      percentage = 0.25f;

    const float stepBetweenHorizontalSegments = 64.0f / (float)wallHeight;

    // offset is needed for walls that are very close to the player so they wont stick to the top of the screen
    // it stays 0 if wall height is smaller than the screen height
    const float offset = (wallHeight > dd->height) ? (wallHeight - dd->height) / 2.0f : 0;

    float horizontalSegment = offset * stepBetweenHorizontalSegments;

    const float c = side ? hitPointX - floorf(hitPointX) : hitPointY - floorf(hitPointY);

    for (int pixel = startPos; pixel < endPos; pixel++) {

      const int verticalSegment = 64 * c;

      const int hpi = (int)horizontalSegment * 64 + verticalSegment; // horizontal pixel index
      horizontalSegment += stepBetweenHorizontalSegments;

      RGB rgb(tileMap[hpi + tOffset]);
      rgb.Multiply(percentage);
      const uint32_t reColor = rgb.ReturnRGB();

      dd->pixels[pixel * dd->width + ray] = reColor;
    }

    // add rays to the map
    // Vector2i hitPoint;
    // hitPoint.x = hitPointX + hitPointX * 8;
    // hitPoint.y = hitPointY + hitPointY * 8;

    // dd->pixels[hitPoint.y * dd->width + hitPoint.x] = YELLOW_COLOR;
    // AddPixelToBuffer(dd, hitPoint.x, hitPoint.y, YELLOW_COLOR);

    // Vector2i playerPos;
    // playerPos.x = player->pos.x + player->pos.x * 8.0f;
    // playerPos.y = player->pos.y + player->pos.y * 8.0f;

    // AddLine(dd, playerPos, hitPoint, YELLOW_COLOR);
  }
}

// Vector2i StartingPositionForCentering(uint32_t *pixelBuffer, int width, int height, int objectWidth, int objectHeight) {
//   Vector2i pos;
//   x = width / 2 - objectWidth / 2;
//   y = height / 2 - objectHeight / 2;
//   return pos;
// }

// struct Vector2i PositionInCorner(int width, int height, int objectWidth,
// int objectHeight)
// {
//   struct Vector2i position;
//   position.x = width / 2 - (objectWidth) / 2;
//   position.y = height / 2 - (objectHeight) / 2;
//   return position;
// }

void DrawMap(DisplayData *dd, int8_t *map, Player *player) {
  const int mapWidth = 16;
  const int mapHeight = 16;

  for (int s = 0; s < mapWidth * mapHeight; s++) {
    const int x = s % mapWidth;
    const int y = s / mapWidth;
    // const int offset = 8;

    // const int xCounter = 0;
    // const int yCounter = 0;

    AddPixelToBuffer(dd, x + x * 8, y + y * 8, RED_COLOR);
  }

  Vector2i playerPosOnMap;
  playerPosOnMap.x = player->pos.x + player->pos.x * 8;
  playerPosOnMap.y = player->pos.y + player->pos.y * 8;

  // for (int i = 0; i < dd->width; i++) {
  //   AddPixelToBuffer(dd, raycastHitPoints[i]->x, raycastHitPoints[i]->y, RED_COLOR);
  // }

  // for (int i = 0; i < mapSize * mapSize; i++) {
  //   const int x = i % mapSize;
  //   const int y = i / mapSize;

  //   // map squares
  //   if (map[i] == 1) {
  //     AddPixelToBuffer(dd, x, y, BLUE_COLOR);
  //   } else if (map[i] == 2) {
  //     AddPixelToBuffer(dd, x, y, WHITE_COLOR);
  //   }

  //   else {
  //     AddPixelToBuffer(dd, x, y, BLACK_COLOR);
  //   }
  // }

  // // draw player arrow in center
  AddLineInDirectionWithArrow(dd, playerPosOnMap, 12.0f, player->rotRad, RED_COLOR);

  // direction arrow for player
  if (player->speed != 0) {
    AddLineInDirectionWithArrow(dd, playerPosOnMap, 8.0f, player->rotRad + player->moveDirRad, GREEN_COLOR);
  }
}

int CalculateAverageFps(int executionTime) {
  const int FPS_HISTORY_SIZE = 8;

  static int fpsHistory[8];

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

void ToggleMap(bool *mapEnabled) { *mapEnabled = !(*mapEnabled); }

int main(int argv, char **args) {
  int windowWidth = 1920;
  int windowHeight = 1080;

  uint32_t *tileMap = LoadTexture("tilemap", 512, 512);
  if (tileMap == NULL) {
    return 1;
  }
  // uint32_t *skybox1 = LoadTexture("skybox1", 512, 256);

  int resScale = 4;

  int width = windowWidth / resScale;
  int height = windowHeight / resScale;

  int size = width * height;

  // const int width = 1000;
  // const int height = 1000;

  // clang-format off
  int8_t map[16 * 16] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 ,1, 1, 1,
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

  // initialize sdl
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    SDL_Log("SDL video could not initialize! SDL_Error: %s", SDL_GetError());
    return EXIT_FAILURE;
  }
  // create window
  SDL_Window *window = SDL_CreateWindow("", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, windowWidth, windowHeight, SDL_WINDOW_SHOWN);
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

  DisplayData dd;
  dd.width = width;
  dd.height = height;
  dd.size = size;

  // RNG
  // srand(time(0));

  // player values
  Player player;
  player.pos.x = 8.0f;
  player.pos.y = 8.0f;

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
  // printf("%ld\n", currentTime);

  // char hwsw[] = "HW";

  // SDL_ShowCursor(true);
  // SDL_SetRelativeMouseMode(true);
  SDL_SetRelativeMouseMode(SDL_TRUE);
  // Vector2i mousePosition;

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
        // player.rotation += (width / 2.0f - event.motion.x) / 128;
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

    // printf("X: %f, Y: %f, r: %f\n", player.x, player.y, player.rot);

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

    // printf("%d\n", map[4]);

    // unlock the texture and render the scene
    SDL_UnlockTexture(texture);
    SDL_RenderCopy(renderer, texture, NULL, NULL);
    SDL_RenderPresent(renderer);

    // printf("execution time: %d\n", executionTime);
    if (limitSpeed) {
      int executionTime = GetMicroTime() - startTime;
      int timeToSleep = 16666 - executionTime;

      if (timeToSleep > 0) {
        std::this_thread::sleep_for(std::chrono::microseconds(timeToSleep));
      }
    }

    long const elapsedTime = GetMicroTime() - currentTime;
    long const executionTimeWithSleep = GetMicroTime() - startTime;

    int const avgFps = CalculateAverageFps(executionTimeWithSleep);
    deltaTime = (GetMicroTime() - startTime) * 60.0f / 1000000.0f;

    // printf("%.6f\n", deltaTime);
    // long frameTime = GetMicroTime() - startTime;
    // float gameSpeed = ((1.0f / frameTime / 60.0f) * 1000000.0f) * deltaTime;

    // Break the loop after 1 second (1,000,000 microseconds)

    if (elapsedTime >= 1000000) {
      // int duration = GetMicroTime() - startTime;
      // printf("elapsed time: %d\n", elapsedTime);

      const string title = to_string(width) + "x" + to_string(height) + " - " + to_string(avgFps) + " fps";

      SDL_SetWindowTitle(window, title.c_str());
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
