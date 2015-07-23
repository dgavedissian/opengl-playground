/*
 * GL Framework
 * Copyright (c) David Avedissian 2014-2015
 */
#ifndef TEXTURE_H
#define TEXTURE_H

class Texture
{
public:
    Texture(const string& file);
    Texture(uint width, uint height, GLuint format = GL_RGB, GLuint type = GL_UNSIGNED_BYTE);
    ~Texture();

    void Bind(uint unit);

    GLuint GetID() const;
    uint GetWidth() const;
    uint GetHeight() const;

private:
    GLuint mTextureID;

    uint mWidth;
    uint mHeight;
};

#endif /* TEXTURE_H */
