/*
 * GL Framework
 * Copyright (c) David Avedissian 2014-2015
 */
#pragma once

#include "CameraMan.h"

#define DEFINE_MAIN_FUNCTION(CLASS) \
    extern "C" int main(int, char**) \
    { \
        return CLASS().Run(#CLASS, WIDTH, HEIGHT); \
    }


class Application
{
public:
    Application();
    ~Application();

	int CreateWindow(const string& windowTitle, uint width, uint height);
    void DestroyWindow();

    int Run(const string& windowTitle, uint width, uint height);

    // Application callbacks
	virtual void Startup() = 0;
	virtual void Shutdown() = 0;
    virtual bool Render() = 0;

    // Optional callbacks
    virtual void OnMouseDown(int button);
    virtual void OnMouseUp(int button);
    virtual void OnMouseMove(const glm::vec2& offset);
    virtual void OnKeyDown(SDL_Keycode kc);
    virtual void OnKeyUp(SDL_Keycode kc);

protected:
    glm::mat4 mViewMatrix;
	glm::mat4 mProjMatrix;
	uint mWindowWidth, mWindowHeight;

    CameraMan mCameraMan;

private:
	SDL_Window* mWindow;
	SDL_GLContext mContext;

private:
	void PrintSDLError();
};
