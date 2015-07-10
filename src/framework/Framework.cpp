/*
 * GL Framework
 * Copyright (c) David Avedissian 2014-2015
 */
#include "Common.h"
#include "Framework.h"

Framework::Framework() : mWindow(nullptr), mWindowWidth(0), mWindowHeight(0)
{
}

Framework::~Framework()
{
    if (mWindow != nullptr)
        DestroyWindow();
}

int Framework::CreateWindow(const string& windowTitle, uint width, uint height)
{
    // Create a window
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        PrintSDLError();
        return 1;
    }

    // Set up GL context
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    // Create the window
    mWindow = SDL_CreateWindow(windowTitle.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                               width, height, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
    if (mWindow == nullptr)
    {
        PrintSDLError();
        return 1;
	}
	
	// Now the window exists without errors, update the member variables
	mWindowWidth = width;
	mWindowHeight = height;

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

void Framework::DestroyWindow()
{
    assert(mWindow != nullptr);
    SDL_DestroyWindow(mWindow);
    SDL_Quit();
}

int Framework::Run(const string& windowTitle, uint width, uint height)
{
	try
	{
		if (CreateWindow(windowTitle, width, height) != 0)
			return 1;
		Startup();

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
					OnKeyDown(e.key.keysym.sym);
					break;

				default:
					break;
				}
			}

			// Render a frame
			if (!Render())
				quit = true;

			// Swap the front and back buffer
			SDL_GL_SwapWindow(mWindow);
		}

		Shutdown();
		DestroyWindow();
		return 0;
	}
	catch (std::exception& e)
	{
		SDL_ShowSimpleMessageBox(0, "Runtime Error", e.what(), NULL);
		return 1;
	}
}

void Framework::PrintSDLError()
{
	cout << "SDL Error: " << SDL_GetError() << endl;
}
