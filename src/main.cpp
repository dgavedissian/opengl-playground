#include <iostream>
#include <SDL.h>

SDL_Window* createWindow(int width, int height)
{
  if (SDL_Init(SDL_INIT_VIDEO) < 0)
  {
    // ERROR
  }
  else
  {
    SDL_Window* window = SDL_CreateWindow(
        "Deferred Lighting", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        width, height, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
    if (window == NULL)
    {
      // ERROR
    }

    return window;
  }
}

void cleanup(SDL_Window* window)
{
  SDL_DestroyWindow(window);
  SDL_Quit();
}

int main()
{
  SDL_Window* window = createWindow(1024, 768);
  cleanup(window);
}
