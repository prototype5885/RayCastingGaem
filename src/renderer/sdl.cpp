#include "sdl.h"

#include "../display.h"
#include <SDL2/SDL.h>
#include <iostream>

SDL_Window *window = nullptr;
SDL_Renderer *renderer = nullptr;
SDL_Texture *sdlTexture = nullptr;

namespace sdl {
void InitSDL(const bool fullscreen, const int windowWidth, const int windowHeight, const bool linearFiltering) {
  using namespace std;

  cout << "Initializing SDL..." << endl;
  int windowMode = SDL_WINDOW_SHOWN;
  if (fullscreen) {
    windowMode = SDL_WINDOW_FULLSCREEN;
  }

  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    throw runtime_error(SDL_GetError());
  }

  window = SDL_CreateWindow("", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, windowWidth, windowHeight, windowMode);
  if (window == nullptr) {
    SDL_Quit();
    throw runtime_error(SDL_GetError());
  }

  // SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);

  renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
  if (renderer == nullptr) {
    SDL_DestroyWindow(window);
    SDL_Quit();
    throw runtime_error(SDL_GetError());
  }

  // set resolution inside the window
  SDL_RenderSetLogicalSize(renderer, display::width, display::height);
  if (linearFiltering) {
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "2");
  }

  // create the texture that will display content in the window
  sdlTexture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, display::width, display::height);
  if (sdlTexture == nullptr) {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    throw runtime_error(SDL_GetError());
  }

  SDL_SetRelativeMouseMode(SDL_TRUE);
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
    SDL_Quit();
    throw std::runtime_error(SDL_GetError());
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
