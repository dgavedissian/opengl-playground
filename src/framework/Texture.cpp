/*
 * GL Framework
 * Copyright (c) David Avedissian 2014-2015
 */
#include "Common.h"
#include "Texture.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

Texture::Texture(const string& filename)
{
    glGenTextures(1, &mTextureID);
    glBindTexture(GL_TEXTURE_2D, mTextureID);

    // Load the image file
    int width, height, numComponents;
    unsigned char* data = stbi_load(filename.c_str(), &width, &height, &numComponents, 4);
    mWidth = width;
    mHeight = height;

    // Filtering
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    // Give image data to OpenGL
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, mWidth, mHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

    stbi_image_free(data);
}

Texture::Texture(uint width, uint height, GLuint format, GLuint type) : mWidth(width), mHeight(height)
{
    glGenTextures(1, &mTextureID);
    glBindTexture(GL_TEXTURE_2D, mTextureID);

    // Filtering
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Create image
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, GL_RGB, type, NULL);
}

Texture::~Texture()
{
    glDeleteTextures(1, &mTextureID);
}

void Texture::Bind(uint unit)
{
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_2D, mTextureID);
}

GLuint Texture::GetID() const
{
    return mTextureID;
}

uint Texture::GetWidth() const
{
    return mWidth;
}

uint Texture::GetHeight() const
{
    return mHeight;
}
