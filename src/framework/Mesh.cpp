/*
 * GL Framework
 * Copyright (c) David Avedissian 2014-2015
 */
#include "Common.h"
#include "Mesh.h"

Mesh::Mesh(vector<GLfloat> vertexData, vector<VertexAttribute> layout)
    : mVertexArrayObject(0),
      mVertexBufferObject(0),
      mElementBufferObject(0),
      mVertexCount(0)
{
    glGenVertexArrays(1, &mVertexArrayObject);
    glBindVertexArray(mVertexArrayObject);

    // Generate vertex buffer
    glGenBuffers(1, &mVertexBufferObject);
    glBindBuffer(GL_ARRAY_BUFFER, mVertexBufferObject);
    glBufferData(GL_ARRAY_BUFFER, vertexData.size() * sizeof(float), vertexData.data(),
		GL_STATIC_DRAW);

    // Calculate vertex size
    uint vertexSize = 0;
    for (auto i = layout.begin(); i != layout.end(); i++)
        vertexSize += i->count;

    // As we know the vertex size, set the vertex count
    mVertexCount = vertexData.size() / vertexSize;

    // Set up vertex layout
    uint attributeCounter = 0;
    uint offset = 0;
    for (auto i = layout.begin(); i != layout.end(); i++)
    {
        glEnableVertexAttribArray(attributeCounter);
        glVertexAttribPointer(attributeCounter, i->count, i->type, GL_FALSE,
                              vertexSize * sizeof(float), (void*)(offset * sizeof(float)));
        attributeCounter++;
        offset += i->count;
    }
}

Mesh::Mesh(vector<GLfloat> vertexData, vector<GLuint> elementData,
                           vector<VertexAttribute> layout)
    : mVertexArrayObject(0),
      mVertexBufferObject(0),
      mElementBufferObject(0),
      mVertexCount(elementData.size())
{
    glGenVertexArrays(1, &mVertexArrayObject);
    glBindVertexArray(mVertexArrayObject);

    // Generate vertex buffer
    glGenBuffers(1, &mVertexBufferObject);
    glBindBuffer(GL_ARRAY_BUFFER, mVertexBufferObject);
    glBufferData(GL_ARRAY_BUFFER, vertexData.size() * sizeof(float), vertexData.data(),
		GL_STATIC_DRAW);

    // Generate element buffer
    glGenBuffers(1, &mElementBufferObject);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mElementBufferObject);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, elementData.size() * sizeof(GLuint), elementData.data(),
		GL_STATIC_DRAW);

    // Calculate vertex size
    uint vertexSize = 0;
    for (auto i = layout.begin(); i != layout.end(); i++)
        vertexSize += i->count;

    // Set up vertex layout
    uint attributeCounter = 0;
    uint offset = 0;
    for (auto i = layout.begin(); i != layout.end(); i++)
    {
        glEnableVertexAttribArray(attributeCounter);
        glVertexAttribPointer(attributeCounter, i->count, i->type, GL_FALSE,
                              vertexSize * sizeof(float), (void*)(offset * sizeof(float)));
        attributeCounter++;
        offset += i->count;
    }
}

Mesh::~Mesh()
{
}

void Mesh::Bind()
{
    glBindVertexArray(mVertexArrayObject);
}

void Mesh::Draw()
{
    if (mElementBufferObject != 0)
    {
        glDrawElements(GL_TRIANGLES, mVertexCount, GL_UNSIGNED_INT, 0);
    }
    else
    {
        glDrawArrays(GL_TRIANGLES, 0, mVertexCount);
    }
}
