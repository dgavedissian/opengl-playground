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
  // Create a window
  if (SDL_Init(SDL_INIT_VIDEO) < 0)
  {
    checkSDLError();
    return 1;
  }

  // Set up GL context
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

  // Create the window
  mWindow = SDL_CreateWindow(
      "Deferred Lighting", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
      width, height, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
  if (mWindow == nullptr)
  {
    checkSDLError();
    return 1;
  }

  // Create the GL context
  mContext = SDL_GL_CreateContext(mWindow);
  SDL_GL_SetSwapInterval(1);
 
  // Set up GLEW
  glewExperimental = GL_TRUE;
  glewInit();

  // get version info
  const GLubyte* renderer = glGetString(GL_RENDERER);
  const GLubyte* version = glGetString(GL_VERSION);
  std::cout << "Renderer: " << renderer << std::endl;
  std::cout << "OpenGL version supported " << version << std::endl;

  // Everything ok
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
 
  // Main loop
  SDL_Event e;
  bool quit = false;
  while (!quit)
  {
    // Handle message pump
    while (SDL_PollEvent(&e) != 0)
    {
      if (e.type == SDL_QUIT)
        quit = true;
    }

    // Render a frame
    glClear(GL_COLOR_BUFFER_BIT);
    render();
    SDL_GL_SwapWindow(mWindow);
  }

  destroyWindow();
  return 0;
}
