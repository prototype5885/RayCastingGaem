#include "sdl.h"

#include "../display.h"
#include <SDL2/SDL.h>
#include <iostream>

using namespace std;

SDL_Window *window = nullptr;
SDL_Renderer *renderer = nullptr;
SDL_Texture *sdlTexture = nullptr;

namespace sdl {
int InitSDL(const bool fullscreen, const int windowWidth, const int windowHeight, const bool linearFiltering) {
  cout << "Initializing SDL..." << endl;
  int windowMode = SDL_WINDOW_SHOWN;
  if (fullscreen) {
    windowMode = SDL_WINDOW_FULLSCREEN;
  }

  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    cerr << SDL_GetError();
    return 1;
  }

  window = SDL_CreateWindow("", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, windowWidth, windowHeight, windowMode);
  if (window == nullptr) {
    SDL_Quit();
    cerr << SDL_GetError();
    return 1;
  }

  // SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);

  renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
  if (renderer == nullptr) {
    SDL_DestroyWindow(window);
    SDL_Quit();
    cerr << SDL_GetError();
    return 1;
  }

  // set resolution inside the window
  SDL_RenderSetLogicalSize(renderer, display::width, display::height);
  if (linearFiltering) {
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "2");
  }

  // create the texture that will display content in the window
  sdlTexture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGB888, SDL_TEXTUREACCESS_STREAMING, display::width, display::height);
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

uint32_t *GetPixelBuffer() {
  uint32_t *pixels;
  int pitch;
  if (SDL_LockTexture(sdlTexture, nullptr, reinterpret_cast<void **>(&pixels), &pitch) != 0) {
    SDL_DestroyTexture(sdlTexture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    printf("SDL_LockTexture Error: %s\n", SDL_GetError());
    SDL_Quit();
  }

  return pixels;
}

void Draw() {
  SDL_UnlockTexture(sdlTexture);
  SDL_RenderCopy(renderer, sdlTexture, nullptr, nullptr);
  SDL_RenderPresent(renderer);
}

void SetWindowTitle(const char *title) { SDL_SetWindowTitle(window, title); }
} // namespace sdl
