/*
 * GL Framework
 * Copyright (c) David Avedissian 2014-2015
 */
#include "common.h"
#include "framework.h"
#include "main.h"

int main()
{
    auto app = make_shared<DeferredShading>();
    return app->run(1024, 768);
}

void DeferredShading::setup()
{
    mShader = loadShader("media/red.vs", "media/red.fs");

    // Create a vertex array object to store vertex buffer and layout
    glGenVertexArrays(1, &mVAO);
    glBindVertexArray(mVAO);

    // Vertices
    static const float vertices[] = {
        -0.5f, -0.5f, 0.0f,
        0.5f, -0.5f, 0.0f,
        0.0f, 0.5f, 0.0f
    };

    // Generate vertex buffer
    glGenBuffers(1, &mVBO);
    glBindBuffer(GL_ARRAY_BUFFER, mVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Set up vertex layout
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(0);
}

void DeferredShading::render()
{
    // Bind VAO
    glBindVertexArray(mVAO);
    glUseProgram(mShader);
    glDrawArrays(GL_TRIANGLES, 0, 3);
}

void DeferredShading::cleanup()
{
    glDeleteProgram(mShader);
}
