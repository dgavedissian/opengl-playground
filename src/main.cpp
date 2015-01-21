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

    // Create VAO
    glGenVertexArrays(1, &mVAO);
    glBindVertexArray(mVAO);

    // Vertices
    static const float vertices[] = {-1.0f, -1.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f};

    // Generate vertex buffer
    glGenBuffers(1, &mVB);
    glBindBuffer(GL_ARRAY_BUFFER, mVB);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
}

void DeferredShading::render()
{
    // Draw vertex buffer
    glEnableVertexAttribArray(0);
    glUseProgram(mShader);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glDisableVertexAttribArray(0);
}

void DeferredShading::cleanup()
{
    glDeleteProgram(mShader);
}
