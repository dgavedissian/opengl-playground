/*
 * GL Framework
 * Copyright (c) David Avedissian 2014-2015
 */
#pragma once

#include "CameraMan.h"

#define DEFINE_MAIN_FUNCTION(CLASS) \
    extern "C" int main(int, char**) \
    { \
        return CLASS().run(#CLASS, WIDTH, HEIGHT); \
    }


class Application
{
public:
    Application();
    ~Application();

	int createWindow(const string& windowTitle, uint width, uint height);
    void destroyWindow();

    int run(const string& windowTitle, uint width, uint height);

    // Application callbacks
	virtual void startup() = 0;
	virtual void shutdown() = 0;
    virtual bool render() = 0;

    // Optional callbacks
    virtual void onMouseDown(int button);
    virtual void onMouseUp(int button);
    virtual void onMouseMove(const glm::vec2& offset);
    virtual void onKeyDown(SDL_Keycode kc);
    virtual void onKeyUp(SDL_Keycode kc);

protected:
    glm::mat4 mViewMatrix;
	glm::mat4 mProjMatrix;
	uint mWindowWidth, mWindowHeight;

    CameraMan mCameraMan;

private:
	SDL_Window* mWindow;
	SDL_GLContext mContext;

private:
	void printSDLError();
};
