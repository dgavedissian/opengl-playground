/*
 * GL Framework
 * Copyright (c) David Avedissian 2014-2015
 */
#include "Common.h"
#include "Application.h"

Application::Application() : mWindow(nullptr), mWindowWidth(0), mWindowHeight(0)
{
}

Application::~Application()
{
    if (mWindow != nullptr)
        destroyWindow();
}

int Application::createWindow(const string& windowTitle, uint width, uint height)
{
    INFO << "Starting " << windowTitle << endl;

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
    mWindow = SDL_CreateWindow(windowTitle.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                               width, height, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
    if (mWindow == nullptr)
    {
        printSDLError();
        return 1;
	}

	// Now the window exists without errors, update the member variables
	mWindowWidth = width;
	mWindowHeight = height;

    // Create the GL context
    mContext = SDL_GL_CreateContext(mWindow);
    SDL_GL_SetSwapInterval(1);

    // Initialise gl3w
    if (gl3wInit())
    {
        ERROR << "Failed to initialise OpenGL" << endl;
        return 1;
    }

    // Get version info
    INFO << "Renderer: " << glGetString(GL_RENDERER) << endl;
    INFO << "OpenGL " << glGetString(GL_VERSION)
         << ", GLSL " << glGetString(GL_SHADING_LANGUAGE_VERSION) << endl;

    // Everything is ok
    return 0;
}

void Application::destroyWindow()
{
    assert(mWindow != nullptr);
    SDL_DestroyWindow(mWindow);
    SDL_Quit();
}

int Application::run(const string& windowTitle, uint width, uint height)
{
	try
	{
		if (createWindow(windowTitle, width, height) != 0)
			return 1;
		startup();

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

                case SDL_MOUSEBUTTONDOWN:
                    onMouseDown(e.button.button);
                    break;

                case SDL_MOUSEBUTTONUP:
                    onMouseUp(e.button.button);
                    break;

                case SDL_MOUSEMOTION:
                    onMouseMove(glm::vec2(e.motion.xrel, e.motion.yrel));
                    break;

				case SDL_KEYDOWN:
					onKeyDown(e.key.keysym.sym);
					break;

                case SDL_KEYUP:
                    onKeyUp(e.key.keysym.sym);
                    break;

				default:
					break;
				}
			}

			// Render a frame
			if (!render())
				quit = true;

			// Swap the front and back buffer
			SDL_GL_SwapWindow(mWindow);
		}

		shutdown();
		destroyWindow();
		return 0;
	}
	catch (std::exception& e)
	{
        ERROR << "Runtime Exception: " << e.what() << endl;
		SDL_ShowSimpleMessageBox(0, "Runtime Error", e.what(), NULL);
		return 1;
	}
}

void Application::onMouseDown(int button)
{
    mCameraMan.onMouseDown(button);
}

void Application::onMouseUp(int button)
{
    mCameraMan.onMouseUp(button);
}

void Application::onMouseMove(const glm::vec2& offset)
{
    mCameraMan.onMouseMove(offset);
}

void Application::onKeyDown(SDL_Keycode kc)
{
    mCameraMan.onKeyDown(kc);
}

void Application::onKeyUp(SDL_Keycode kc)
{
    mCameraMan.onKeyUp(kc);
}

void Application::printSDLError()
{
	ERROR << "SDL Error: " << SDL_GetError() << endl;
}
