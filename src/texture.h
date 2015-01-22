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
    ~Texture();

    void bind();

private:
    GLuint mTextureID;

    int mWidth;
    int mHeight;
};

#endif /* TEXTURE_H */
