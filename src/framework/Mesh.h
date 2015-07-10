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

class Mesh
{
public:
    Mesh(vector<GLfloat> vertexData, vector<VertexAttribute> layout);
    Mesh(vector<GLfloat> vertexData, vector<GLuint> elementData,
                 vector<VertexAttribute> layout);
    ~Mesh();

    void Bind();
    void Draw();

private:
    GLuint mVertexArrayObject, mVertexBufferObject, mElementBufferObject;
    uint mVertexCount;
};

#endif /* VERTEXBUFFER_H */
