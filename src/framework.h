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

    virtual void setup() = 0;
    virtual void render() = 0;
    virtual void cleanup() = 0;

private:
    SDL_Window* mWindow;
    SDL_GLContext mContext;
};

#endif /* FRAMEWORK_H */
