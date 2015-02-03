/*
 * GL Framework
 * Copyright (c) David Avedissian 2014-2015
 */
#ifndef FRAMEWORK_H
#define FRAMEWORK_H

class Framework
{
public:
    Framework();
    ~Framework();

    void printSDLError();
    int createWindow(uint width, uint height);
    void destroyWindow();

    int run(uint width, uint height);

    // Application callbacks
    virtual void setup() = 0;
    virtual bool drawFrame() = 0;
    virtual void cleanup() = 0;
    virtual void onKeyDown(SDL_Keycode kc) = 0;

protected:
    glm::mat4 mViewMatrix;
    glm::mat4 mProjMatrix;

private:
    SDL_Window* mWindow;
    SDL_GLContext mContext;
};

#endif /* FRAMEWORK_H */
