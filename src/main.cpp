/*
 * GL Framework
 * Copyright (c) David Avedissian 2014-2015
 */
#include "common.h"
#include "framework.h"

#include "framebuffer.h"
#include "shader.h"
#include "texture.h"
#include "vertexbuffer.h"

#include "main.h"

#define WIDTH 1024
#define HEIGHT 768

int main()
{
    return DeferredShading().run(WIDTH, HEIGHT);
}

void DeferredShading::setup()
{
    // Quad mesh data
    vector<float> quadVertices =
    {
        -1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
        1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
        -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
        1.0f, -1.0f, 0.0f, 1.0f, 0.0f
    };
    vector<GLuint> quadElements = {0, 2, 1, 1, 2, 3};
    vector<VertexAttribute> quadLayout = {{3, GL_FLOAT}, {2, GL_FLOAT}};

    // Set up post processing
    mFb = new Framebuffer(WIDTH, HEIGHT, 3);
    mPostShader = new Shader("media/quad.vs", "media/post.fs");
    mQuad = new VertexBuffer(quadVertices, quadElements, quadLayout);
    mPostShader->bind();
    mPostShader->setUniform<int>("gb0", 0);
    mPostShader->setUniform<int>("gb1", 1);
    mPostShader->setUniform<int>("gb2", 2);

    // Set up scene
    mShader = new Shader("media/sample.vs", "media/sample.fs");
    mTexture = new Texture("media/wall.jpg");

    // Vertices
    vector<float> vertices =
    {
        -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
        1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,
        -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
        1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f
    };
    vector<GLuint> elements = {0, 2, 1, 1, 2, 3};
    vector<VertexAttribute> layout = {{3, GL_FLOAT}, {3, GL_FLOAT}, {2, GL_FLOAT}};

    mMesh = new VertexBuffer(vertices, elements, layout);
}

bool DeferredShading::drawFrame()
{
    // Start remdering to the framebuffer
    mFb->bind();
    glEnable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    {
        // Set up the shader parameters
        mShader->bind();
        static glm::mat4 model;
        model = glm::rotate(model, (float)clock() / (float)CLOCKS_PER_SEC * 0.05f, glm::vec3(0.0f, 1.0f, 0.0f));
        glm::mat4 view = glm::lookAt(
            glm::vec3(0.0f, 0.0f, 2.0f),
            glm::vec3(0.0f, 0.0f, 0.0f),
            glm::vec3(0.0f, 1.0f, 0.0f));
        glm::mat4 proj = glm::perspective(45.0f, (float)WIDTH / HEIGHT, 0.1f, 100.0f);
        mShader->setUniform("modelViewProj", proj * view * model);
        mShader->setUniform("model", model);

        // Draw the mesh
        glActiveTexture(GL_TEXTURE0);
        mTexture->bind();
        mMesh->bind();
        mMesh->draw();
    }

    // Render fullscreen quad
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDisable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT);
    {
        static float val = 0.0f;
        val += 0.01f;
        mPostShader->bind();
       
        glActiveTexture(GL_TEXTURE0);
        mFb->getColourBuffer(0)->bind();
        glActiveTexture(GL_TEXTURE1);
        mFb->getColourBuffer(1)->bind();
        glActiveTexture(GL_TEXTURE2);
        mFb->getColourBuffer(2)->bind();
        mQuad->bind();
        mQuad->draw();
    }

    // Check for GL errors
    GLuint err = glGetError();
    if (err != 0)
    {
        cerr << "glGetError() returned " << err << endl;
        return false;
    }

    return true;
}

void DeferredShading::cleanup()
{
    delete mShader;
    delete mTexture;
    delete mMesh;

    delete mPostShader;
    delete mQuad;
    delete mFb;
}

void DeferredShading::onKeyDown(SDL_Keycode kc)
{
    cout << "Key " << kc << " pressed!" << endl;
}
