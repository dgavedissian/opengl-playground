/*
 * GL Framework
 * Copyright (c) David Avedissian 2014-2015
 */
#include "common.h"
#include "framework.h"

Framework::Framework() : mWindow(nullptr)
{
}

Framework::~Framework()
{
    if (mWindow != nullptr)
        destroyWindow();
}

void Framework::printSDLError()
{
    cout << "SDL Error: " << SDL_GetError() << endl;
}

int Framework::createWindow(uint width, uint height)
{
    // Create a window
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        printSDLError();
        return 1;
    }

    // Set up GL context
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    // Create the window
    mWindow = SDL_CreateWindow("Deferred Lighting", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                               width, height, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
    if (mWindow == nullptr)
    {
        printSDLError();
        return 1;
    }

    // Create the GL context
    mContext = SDL_GL_CreateContext(mWindow);
    SDL_GL_SetSwapInterval(1);

    // Set up GLEW - OS X doesn't require glew: http://stackoverflow.com/a/11213354
#ifndef __APPLE__
    glewExperimental = GL_TRUE;
    GLenum err = glewInit();
    if (err != GLEW_OK)
    {
        cout << "GLEW Error: " << glewGetErrorString(err);
        return 1;
    }
#endif

    // get version info
    const GLubyte* renderer = glGetString(GL_RENDERER);
    const GLubyte* version = glGetString(GL_VERSION);
    cout << "Renderer: " << renderer << endl;
    cout << "OpenGL version supported " << version << endl;

    // Everything ok
    return 0;
}

void Framework::destroyWindow()
{
    assert(mWindow != nullptr);
    SDL_DestroyWindow(mWindow);
    SDL_Quit();
}

int Framework::run(uint width, uint height)
{
    if (createWindow(width, height) != 0)
        return 1;
    setup();

    // Main loop
    SDL_Event e;
    bool quit = false;
    while (!quit)
    {
        // Handle message pump
        while (SDL_PollEvent(&e) != 0)
        {
            switch (e.type)
            {
            case SDL_QUIT:
                quit = true;
                break;

            case SDL_KEYDOWN:
                onKeyDown(e.key.keysym.sym);
                break;

            default:
                break;
            }
        }

        // Render a frame
        if (!drawFrame())
            quit = true;

        // Swap the front and backbuffer
        SDL_GL_SwapWindow(mWindow);
    }

    cleanup();
    destroyWindow();
    return 0;
}
