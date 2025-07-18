#ifndef SDL_H
#define SDL_H
#include <cstdint>

namespace sdl {
int InitSDL(bool fullscreen, int windowWidth, int windowHeight, bool linearFiltering);
uint32_t *GetPixelBuffer();
void Draw();
void SetWindowTitle(const char *title);
void Quit();
} // namespace sdl

#endif // SDL_H
