/*
 * GL Framework
 * Copyright (c) David Avedissian 2014-2015
 */
#pragma once

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
