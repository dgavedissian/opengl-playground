/*
 * GL Framework
 * Copyright (c) David Avedissian 2014-2015
 */
#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

class Texture;

class Framebuffer
{
public:
    Framebuffer(shared_ptr<Texture> colourBuffer);
    ~Framebuffer();

    void bind();

private:
    uint mWidth, mHeight;
    shared_ptr<Texture> mColourBuffer;
    GLuint mFramebuffer, mDepthBuffer;
};

#endif /* FRAMEBUFFER_H */
