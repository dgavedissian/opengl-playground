/*
 * GL Framework
 * Copyright (c) David Avedissian 2014-2015
 */
#include "Common.h"
#include "Texture.h"

#include <FreeImage.h>

Texture::Texture(const string& filename)
{
    glGenTextures(1, &mTextureID);
    glBindTexture(GL_TEXTURE_2D, mTextureID);

    // Load the image file
    FIBITMAP* bitmap = FreeImage_Load(FreeImage_GetFileType(filename.c_str(), 0), filename.c_str());
    FIBITMAP* image = FreeImage_ConvertTo32Bits(bitmap);
    FreeImage_Unload(bitmap);
    
    // Grab the raw data
    mWidth = FreeImage_GetWidth(image);
    mHeight = FreeImage_GetHeight(image);
    int scanWidth = FreeImage_GetPitch(image);
    BYTE *data = new BYTE[mHeight * scanWidth];
    FreeImage_ConvertToRawBits(data, image, scanWidth, 32, FI_RGBA_RED_MASK, FI_RGBA_GREEN_MASK, FI_RGBA_BLUE_MASK, TRUE);
    FreeImage_Unload(image);
    
    // Give image data to OpenGL
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, mWidth, mHeight, 0, GL_BGRA, GL_UNSIGNED_BYTE, data);
    delete [] data;

    // Filtering
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Generate mipmaps
    glGenerateMipmap(GL_TEXTURE_2D);
}

Texture::Texture(uint width, uint height, GLuint format, GLuint type) : mWidth(width), mHeight(height)
{
    glGenTextures(1, &mTextureID);
    glBindTexture(GL_TEXTURE_2D, mTextureID);

    // Create image
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, GL_RGB, type, NULL);

    // Filtering
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

Texture::~Texture()
{
    glDeleteTextures(1, &mTextureID);
}

void Texture::bind()
{
    glBindTexture(GL_TEXTURE_2D, mTextureID);
}

GLuint Texture::getID() const
{
    return mTextureID;
}

uint Texture::getWidth() const
{
    return mWidth;
}

uint Texture::getHeight() const
{
    return mHeight;
}
