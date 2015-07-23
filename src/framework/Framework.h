/*
 * GL Framework
 * Copyright (c) David Avedissian 2014-2015
 */
#ifndef FRAMEWORK_H
#define FRAMEWORK_H

// This macro conflicts with the framework
#ifdef CreateWindow
#   undef CreateWindow
#endif

class Framework
{
public:
    Framework();
    ~Framework();

	int CreateWindow(const string& windowTitle, uint width, uint height);
    void DestroyWindow();

    int Run(const string& windowTitle, uint width, uint height);

    // Application callbacks
	virtual void Startup() = 0;
	virtual void Shutdown() = 0;
    virtual bool Render() = 0;
    virtual void OnKeyDown(SDL_Keycode kc) = 0;

protected:
    glm::mat4 mViewMatrix;
	glm::mat4 mProjMatrix;
	uint mWindowWidth, mWindowHeight;

private:
	SDL_Window* mWindow;
	SDL_GLContext mContext;

private:
	void PrintSDLError();
};

#endif /* FRAMEWORK_H */
