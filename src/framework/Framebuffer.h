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
    Framebuffer(uint width, uint height, uint textureCount);
    ~Framebuffer();

    void Bind();

    Texture* GetColourBuffer(uint i);

private:
    uint mWidth, mHeight;

    vector<unique_ptr<Texture>> mTextures;
    GLuint mFramebuffer;
    GLuint mDepthBuffer;
};

#endif /* FRAMEBUFFER_H */
