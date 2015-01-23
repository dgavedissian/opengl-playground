/*
 * GL Framework
 * Copyright (c) David Avedissian 2014-2015
 */
#include "common.h"
#include "mesh.h"

Mesh::Builder::Builder()
{
}

Mesh::Builder::~Builder()
{
}

Mesh::Builder& Mesh::Builder::vertices(float* data, uint size)
{
    mVertexData = data;
    mVertexDataSize = size;
    return *this;
}

Mesh::Builder& Mesh::Builder::elements(uint* data, uint size)
{
    mElementData = data;
    mElementDataSize = size;
    return *this;
}

shared_ptr<Mesh> Mesh::Builder::build()
{
    return make_shared<Mesh>(*this);
}

Mesh::Mesh(Builder& builder) : mVertexCount(builder.mElementDataSize / sizeof(uint))
{
    glGenVertexArrays(1, &mVao);
    glBindVertexArray(mVao);

    // Generate vertex buffer
    glGenBuffers(1, &mVbo);
    glBindBuffer(GL_ARRAY_BUFFER, mVbo);
    glBufferData(GL_ARRAY_BUFFER, builder.mVertexDataSize, builder.mVertexData, GL_STATIC_DRAW);

    // Generate element buffer
    glGenBuffers(1, &mEbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEbo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, builder.mElementDataSize, builder.mElementData, GL_STATIC_DRAW);

    // Set up vertex layout
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), NULL);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
}

Mesh::~Mesh()
{
}

void Mesh::bind()
{
    glBindVertexArray(mVao);
}

void Mesh::draw()
{
    glDrawElements(GL_TRIANGLES, mVertexCount, GL_UNSIGNED_INT, 0);
}
