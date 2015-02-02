/*
 * GL Framework
 * Copyright (c) David Avedissian 2014-2015
 */
#ifndef VERTEXBUFFER_H
#define VERTEXBUFFER_H

struct VertexAttribute
{
    uint count;
    GLenum type;
};

class VertexBuffer
{
public:
    VertexBuffer(vector<float> vertexData, vector<GLuint> elementData,
                 vector<VertexAttribute> layout);
    ~VertexBuffer();

    void bind();
    void draw();

private:
    GLuint mVao, mVbo, mEbo;
    uint mVertexCount;
};

#endif /* VERTEXBUFFER_H */
