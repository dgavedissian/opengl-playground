/*
 * GL Framework
 * Copyright (c) David Avedissian 2014-2015
 */
#include "common.h"
#include "framework.h"
#include "shader.h"
#include "texture.h"
#include "mesh.h"
#include "framebuffer.h"
#include "main.h"

int main()
{
    return DeferredShading().run(1024, 768);
}

void DeferredShading::setup()
{
    // Quad mesh data
    float quadVertices[] = {
        -1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
        1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
        -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
        1.0f, -1.0f, 0.0f, 1.0f, 0.0f
    };
    GLuint quadElements[] = { 0, 2, 1, 1, 2, 3 };

    // Set up post processing
    mFbColourTex = make_shared<Texture>(1024, 768);
    mFb = make_shared<Framebuffer>(mFbColourTex);
    mPostShader = Shader::Builder()
        .vs("media/quad.vs")
        .fs("media/post.fs")
        .link();
    mQuad = Mesh::Builder()
        .vertices(quadVertices, sizeof(quadVertices))
        .elements(quadElements, sizeof(quadElements))
        .build();

    // Set up scene
    mShader = Shader::Builder()
        .vs("media/sample.vs")
        .fs("media/sample.fs")
        .link();
    mTexture = make_shared<Texture>("media/sample.png");

    // Vertices
    float vertices[] = {
        -1.0f, 1.0f, 0.0f, 0.0f, 0.0f,
        1.0f, 1.0f, 0.0f, 1.0f, 0.0f,
        -1.0f, -1.0f, 0.0f, 0.0f, 1.0f,
        1.0f, -1.0f, 0.0f, 1.0f, 1.0f
    };

    mMesh = Mesh::Builder()
        .vertices(vertices, sizeof(vertices))
        .elements(quadElements, sizeof(quadElements))
        .build();
}

void DeferredShading::render()
{
    // Start remdering to the framebuffer
    mFb->bind();
    glEnable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    {
        // Bind object data
        mShader->bind();
        mTexture->bind();
        mMesh->bind();

        // Set shader uniforms
        static glm::mat4 model;
        //model = glm::rotate(model, (float)clock() / (float)CLOCKS_PER_SEC * 0.1f, glm::vec3(0.0f, 0.0f, 1.0f));
        glm::mat4 view = glm::lookAt(
            glm::vec3(0.0f, 1.0f, 2.0f),
            glm::vec3(0.0f, 0.0f, 0.0f),
            glm::vec3(0.0f, 1.0f, 0.0f));
        glm::mat4 proj = glm::perspective(45.0f, 1024.0f / 768.0f, 0.1f, 100.0f);
        mShader->setUniform(mShader->getUniform("modelViewProj"), proj * view * model);

        // Draw the mesh
        mMesh->draw();
    }

    // Render fullscreen quad
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDisable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT);
    {
        mPostShader->bind();
        mFbColourTex->bind();
        mQuad->bind();
        
        static float val = 0.0f;
        val += 0.01f;
        mPostShader->setUniform(mPostShader->getUniform("val"), val);
        
        mQuad->draw();
    }

    // Check for GL errors
    GLuint err = glGetError();
    if (err != 0)
        cout << "GL Error: " << err << endl;
}

void DeferredShading::cleanup()
{
    mMesh.reset();
    mTexture.reset();
    mShader.reset();
}
