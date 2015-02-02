/*
 * GL Framework
 * Copyright (c) David Avedissian 2014-2015
 */
#include "common.h"
#include "vertexBuffer.h"

VertexBuffer::VertexBuffer(vector<float> vertexData, vector<GLuint> elementData,
                           vector<VertexAttribute> layout)
    : mVertexCount(elementData.size())
{
    glGenVertexArrays(1, &mVao);
    glBindVertexArray(mVao);

    // Generate vertex buffer
    glGenBuffers(1, &mVbo);
    glBindBuffer(GL_ARRAY_BUFFER, mVbo);
    glBufferData(GL_ARRAY_BUFFER, vertexData.size() * sizeof(float), vertexData.data(), GL_STATIC_DRAW);

    // Generate element buffer
    glGenBuffers(1, &mEbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEbo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, elementData.size() * sizeof(GLuint), elementData.data(), GL_STATIC_DRAW);

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

VertexBuffer::~VertexBuffer()
{
}

void VertexBuffer::bind()
{
    glBindVertexArray(mVao);
}

void VertexBuffer::draw()
{
    glDrawElements(GL_TRIANGLES, mVertexCount, GL_UNSIGNED_INT, 0);
}
