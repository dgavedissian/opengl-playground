#include "common.h"
#include "framework.h"

Framework::Framework()
  : mWindow(nullptr)
{
}

Framework::~Framework()
{
  if (mWindow != nullptr)
    destroyWindow();
}

void Framework::checkSDLError()
{
  std::cout << "SDL Error: " << SDL_GetError() << std::endl;
}

int Framework::createWindow(unsigned int width, unsigned int height)
{
  if (SDL_Init(SDL_INIT_VIDEO) < 0)
  {
    checkSDLError();
    return 1;
  }
  else
  {
    mWindow = SDL_CreateWindow(
        "Deferred Lighting", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        width, height, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
    if (mWindow == nullptr)
    {
      checkSDLError();
      return 1;
    }
  }

  return 0;
}

void Framework::destroyWindow()
{
  assert(mWindow != nullptr);
  SDL_DestroyWindow(mWindow);
  SDL_Quit();
}

int Framework::run(unsigned int width, unsigned int height)
{
  if (createWindow(width, height) != 0)
    return 1;
  destroyWindow();
  return 0;
}
