/*
 * GL Framework
 * Copyright (c) David Avedissian 2014-2015
 */
#include "common.h"
#include "texture.h"
#include "framebuffer.h"

Framebuffer::Framebuffer(uint width, uint height, uint textureCount)
    : mWidth(width),
      mHeight(height)
{
    glGenFramebuffers(1, &mFramebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, mFramebuffer);

    // Bind colour buffers
    for (uint i = 0; i < textureCount; i++)
    {
        Texture* texture = new Texture(width, height, GL_RGB32F, GL_FLOAT);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D,
                               texture->getID(), 0);
        mTextures.emplace_back(texture);
    }

    // Create depth buffer
    glGenRenderbuffers(1, &mDepthBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, mDepthBuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, mDepthBuffer);

    // Set the list of draw buffers
    vector<GLenum> drawBuffers;
    for (uint i = 0; i < textureCount; i++)
        drawBuffers.push_back(GL_COLOR_ATTACHMENT0 + i);
    glDrawBuffers(textureCount, drawBuffers.data());

    // Check for errors
    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (status != GL_FRAMEBUFFER_COMPLETE)
    {
        stringstream err;
        err << "The framebuffer is not complete. Status: " << status << endl;
        throw std::runtime_error(err.str());
    }

    // Unbind
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

Framebuffer::~Framebuffer()
{
    mTextures.clear();
    glDeleteFramebuffers(1, &mFramebuffer);
}

void Framebuffer::bind()
{
    glBindFramebuffer(GL_FRAMEBUFFER, mFramebuffer);
}

Texture* Framebuffer::getColourBuffer(uint i)
{
    return mTextures[i].get();
}
