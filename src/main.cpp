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

#define WIDTH 1024
#define HEIGHT 768

vector<GLfloat> generateBox(float halfSize);

class PointLight
{
public:
    PointLight(float range)
    {
        mVertices = new VertexBuffer(generateBox(range), {{3, GL_FLOAT}, {3, GL_FLOAT}});
        mShader = new Shader("media/light_pass.vs", "media/point_light_pass.fs");
    }

    ~PointLight()
    {
        delete mShader;
        delete mVertices;
    }

    void setPosition(const glm::vec3& position)
    {
        mWorld = glm::translate(glm::mat4(), position);
    }

    void draw(const glm::mat4& view, const glm::mat4& proj)
    {
        glm::mat4 wvp = proj * view * mWorld;

        // Bind shader
        mShader->bind();
        mShader->setUniform("worldViewProj", wvp);
        
        // Draw light
        mVertices->bind();
        mVertices->draw();
    }

private:
    VertexBuffer* mVertices;
    Shader* mShader;

    glm::mat4 mWorld;

};

class DeferredShading : public Framework
{
private:
    // Post process
    Framebuffer* mFb;
    VertexBuffer* mQuad;
    Shader* mPostShader;

    // Mesh
    VertexBuffer* mMesh;
    Texture* mTexture;
    Shader* mShader;

    // Light
    PointLight* light;

public:
    virtual void setup()
    {
        // Quad mesh data
        vector<GLfloat> quadVertices =
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
        vector<VertexAttribute> layout = {{3, GL_FLOAT}, {3, GL_FLOAT}};
        mMesh = new VertexBuffer(generateBox(0.5f), layout);

        // Light
        light = new PointLight(1.0);
        light->setPosition(glm::vec3(2.0f, 0.0f, 0.0f));
    }

    virtual bool drawFrame()
    {
        mViewMatrix = glm::lookAt(
            glm::vec3(0.0f, 0.0f, 2.0f),
            glm::vec3(0.0f, 0.0f, 0.0f),
            glm::vec3(0.0f, 1.0f, 0.0f));
        mProjMatrix = glm::perspective(45.0f, (float)WIDTH / HEIGHT, 0.1f, 100.0f);
        
        // Start rendering to the framebuffer
        mFb->bind();
        glEnable(GL_DEPTH_TEST);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        {
            // Set up the shader parameters
            mShader->bind();
            static glm::mat4 world;
            world = glm::rotate(world, (float)clock() / (float)CLOCKS_PER_SEC * 0.05f, glm::vec3(0.0f, 1.0f, 0.0f));
            mShader->setUniform("worldViewProj", mProjMatrix * mViewMatrix * world);
            mShader->setUniform("world", world);

            // Draw the mesh
            glActiveTexture(GL_TEXTURE0);
            mTexture->bind();
            mMesh->bind();
            mMesh->draw();
        }
        glDisable(GL_DEPTH_TEST);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // Draw lights
        glEnable(GL_BLEND);
        glBlendEquation(GL_FUNC_ADD);
        glBlendFunc(GL_ONE, GL_ONE);
        glClear(GL_COLOR_BUFFER_BIT);
        {
            // Bind G-Buffer
            glActiveTexture(GL_TEXTURE0);
            mFb->getColourBuffer(0)->bind();
            glActiveTexture(GL_TEXTURE1);
            mFb->getColourBuffer(1)->bind();
            glActiveTexture(GL_TEXTURE2);
            mFb->getColourBuffer(2)->bind();

            // Draw point light
            light->draw(mViewMatrix, mProjMatrix);
        }
        glDisable(GL_BLEND);

        // Check for GL errors
        GLuint err = glGetError();
        if (err != 0)
        {
            cerr << "glGetError() returned " << err << endl;
            return false;
        }

        return true;
    }

    virtual void cleanup()
    {
        delete mShader;
        delete mTexture;
        delete mMesh;

        delete mPostShader;
        delete mQuad;
        delete mFb;
    }

    virtual void onKeyDown(SDL_Keycode kc)
    {
        cout << "Key " << kc << " pressed!" << endl;
    }
};

int main()
{
    return DeferredShading().run(WIDTH, HEIGHT);
}

vector<GLfloat> generateBox(float halfSize)
{
    return {
        -halfSize, -halfSize, -halfSize,  0.0f,  0.0f, -1.0f,
        halfSize, -halfSize, -halfSize,  0.0f,  0.0f, -1.0f,
        halfSize,  halfSize, -halfSize,  0.0f,  0.0f, -1.0f,
        halfSize,  halfSize, -halfSize,  0.0f,  0.0f, -1.0f,
        -halfSize,  halfSize, -halfSize,  0.0f,  0.0f, -1.0f,
        -halfSize, -halfSize, -halfSize,  0.0f,  0.0f, -1.0f,

        -halfSize, -halfSize,  halfSize,  0.0f,  0.0f, 1.0f,
        halfSize, -halfSize,  halfSize,  0.0f,  0.0f, 1.0f,
        halfSize,  halfSize,  halfSize,  0.0f,  0.0f, 1.0f,
        halfSize,  halfSize,  halfSize,  0.0f,  0.0f, 1.0f,
        -halfSize,  halfSize,  halfSize,  0.0f,  0.0f, 1.0f,
        -halfSize, -halfSize,  halfSize,  0.0f,  0.0f, 1.0f,

        -halfSize,  halfSize,  halfSize, -1.0f,  0.0f,  0.0f,
        -halfSize,  halfSize, -halfSize, -1.0f,  0.0f,  0.0f,
        -halfSize, -halfSize, -halfSize, -1.0f,  0.0f,  0.0f,
        -halfSize, -halfSize, -halfSize, -1.0f,  0.0f,  0.0f,
        -halfSize, -halfSize,  halfSize, -1.0f,  0.0f,  0.0f,
        -halfSize,  halfSize,  halfSize, -1.0f,  0.0f,  0.0f,

        halfSize,  halfSize,  halfSize,  1.0f,  0.0f,  0.0f,
        halfSize,  halfSize, -halfSize,  1.0f,  0.0f,  0.0f,
        halfSize, -halfSize, -halfSize,  1.0f,  0.0f,  0.0f,
        halfSize, -halfSize, -halfSize,  1.0f,  0.0f,  0.0f,
        halfSize, -halfSize,  halfSize,  1.0f,  0.0f,  0.0f,
        halfSize,  halfSize,  halfSize,  1.0f,  0.0f,  0.0f,

        -halfSize, -halfSize, -halfSize,  0.0f, -1.0f,  0.0f,
        halfSize, -halfSize, -halfSize,  0.0f, -1.0f,  0.0f,
        halfSize, -halfSize,  halfSize,  0.0f, -1.0f,  0.0f,
        halfSize, -halfSize,  halfSize,  0.0f, -1.0f,  0.0f,
        -halfSize, -halfSize,  halfSize,  0.0f, -1.0f,  0.0f,
        -halfSize, -halfSize, -halfSize,  0.0f, -1.0f,  0.0f,

        -halfSize,  halfSize, -halfSize,  0.0f,  1.0f,  0.0f,
        halfSize,  halfSize, -halfSize,  0.0f,  1.0f,  0.0f,
        halfSize,  halfSize,  halfSize,  0.0f,  1.0f,  0.0f,
        halfSize,  halfSize,  halfSize,  0.0f,  1.0f,  0.0f,
        -halfSize,  halfSize,  halfSize,  0.0f,  1.0f,  0.0f,
        -halfSize,  halfSize, -halfSize,  0.0f,  1.0f,  0.0f
    };
}

