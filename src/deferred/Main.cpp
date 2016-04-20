/*
 * GL Framework
 * Copyright (c) David Avedissian 2014-2015
 */
#include <ctime>

#include "framework/Common.h"
#include "framework/Application.h"
#include "framework/Framebuffer.h"
#include "framework/Shader.h"
#include "framework/Texture.h"
#include "framework/Mesh.h"

#define WIDTH 1024
#define HEIGHT 768

Mesh* generateFullscreenQuad();
Mesh* generateBox(float halfSize);
Mesh* generateLightSphere(float radius, int rings, int segments);

float timeSinceEpoch()
{
	return (float)clock() / (float)CLOCKS_PER_SEC;
}

class PointLight
{
public:
    PointLight(float a0, float a1, float a2)
        : mAtten0(a0),
          mAtten1(a1),
          mAtten2(a2)
    {
        mShader = new Shader("media/light_pass.vs", "media/point_light_pass.fs");
        mShader->bind();
        mShader->setUniform("screenSize", glm::vec2(WIDTH, HEIGHT));
        mShader->setUniform("gb0", 0);
        mShader->setUniform("gb1", 1);
        mShader->setUniform("gb2", 2);
        mShader->setUniform("constant", a0);
        mShader->setUniform("linear", a1);
        mShader->setUniform("exponent", a2);

        // Calculate range
        // Solve 'a2 * d^2 + a1 * d + a0 = 256' for d
        // 256 is the number of distinct light levels in an 8 bit component (2^8)
        float range;
        if (a2 == 0.0f)
        {
            if (a1 == 0.0f)
            {
                // For constant attenuation point lights, set the range to infinity
                range = 100000000000000000.0f;
            }
            else
            {
                range = (256.0f - a0) / a1;
            }
        }
        else
        {
            range = (-a1 + sqrtf(a1 * a1 - 4.0f * a2 * (a0 - 256.0f))) / (2.0f * a2);
        }

		mVertices = generateLightSphere(range, 8, 8);
    }

    ~PointLight()
    {
        delete mShader;
        delete mVertices;
    }

    void setPosition(const glm::vec3& position)
    {
        mPosition = position;
        mWorld = glm::translate(glm::mat4(), position);
    }

    void draw(const glm::mat4& view, const glm::mat4& proj)
    {
        glm::mat4 wvp = proj * view * mWorld;

        // Bind shader
        mShader->bind();
        mShader->setUniform("worldViewProj", wvp);
        mShader->setUniform("lightPos", mPosition);

        // Draw light
        mVertices->bind();
        mVertices->draw();
    }

private:
    Mesh* mVertices;
    Shader* mShader;

    float mAtten0;
    float mAtten1;
    float mAtten2;

    glm::vec3 mPosition;
    glm::mat4 mWorld;

};

class DeferredShadingApp : public Application
{
private:
    // Post process
    Framebuffer* mGBuffer;
    Mesh* mQuad;
    Shader* mPostShader;

    // Mesh
    Mesh* mMesh;
    Texture* mTexture;
    Shader* mShader;

    // Light
    vector<PointLight*> lights;

public:
    virtual void startup() override
    {
        mCameraMan.setPosition(glm::vec3(0.0f, 1.0f, 2.5f));
        mProjMatrix = glm::perspective(45.0f, (float)WIDTH / HEIGHT, 0.1f, 10000.0f);

		// Set up the g-buffer
		mGBuffer = new Framebuffer(mWindowWidth, mWindowHeight, 3);

        // Set up post processing
		mQuad = generateFullscreenQuad();
        mPostShader = new Shader("media/quad.vs", "media/post.fs");
        mPostShader->bind();
        mPostShader->setUniform("gb0", 0);
        mPostShader->setUniform("gb1", 1);
        mPostShader->setUniform("gb2", 2);

        // Set up scene
        mShader = new Shader("media/sample.vs", "media/sample.fs");
        mTexture = new Texture("media/wall.jpg");

        // Scene
        mMesh = generateBox(0.5f);

        // Lights
        for (int x = -1; x <= 1; x++)
        {
            for (int y = -1; y <= 1; y++)
            {
                for (int z = -1; z <= 1; z++)
                {
                    if (!(x == y == z))
                        continue;
                    PointLight* light = new PointLight(0.75f, 0.0f, 1.0f);
                    light->setPosition(glm::vec3(x, y, z) * 0.6f);
                    lights.push_back(light);
                }
            }
        }
    }

	virtual bool render() override
    {
        // Update the camera
        mCameraMan.update(0.0f);

        // Start rendering to the g-buffer
        mGBuffer->bind();
        glEnable(GL_DEPTH_TEST);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        {
            // Set up the shader parameters
            mShader->bind();
            static glm::mat4 world;
			world = glm::rotate(world, 0.01f, glm::vec3(0.0f, 1.0f, 0.0f));
            mShader->setUniform("worldViewProj", mProjMatrix * mCameraMan.getViewMatrix() * world);
            mShader->setUniform("world", world);

            // Draw the mesh
            glActiveTexture(GL_TEXTURE0);
            mTexture->bind(0);
            mMesh->bind();
            mMesh->draw();
        }
        glDisable(GL_DEPTH_TEST);

		// Draw lights
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glEnable(GL_BLEND);
        glBlendEquation(GL_FUNC_ADD);
        glBlendFunc(GL_ONE, GL_ONE);
        glClear(GL_COLOR_BUFFER_BIT);
        {
            // Bind G-Buffer
            mGBuffer->getColourBuffer(0)->bind(0);
            mGBuffer->getColourBuffer(1)->bind(1);
            mGBuffer->getColourBuffer(2)->bind(2);

            // Draw point light
            for (auto i = lights.begin(); i != lights.end(); i++)
                (*i)->draw(mCameraMan.getViewMatrix(), mProjMatrix);
        }
        glDisable(GL_BLEND);

        // Check for GL errors
        GLuint err = glGetError();
        if (err != 0)
        {
            ERROR << "glGetError() returned " << err << endl;
            return false;
        }

        return true;
    }

	virtual void shutdown() override
    {
        delete mShader;
        delete mTexture;
        delete mMesh;

        delete mPostShader;
        delete mQuad;
        delete mGBuffer;
    }
};

DEFINE_MAIN_FUNCTION(DeferredShadingApp)

Mesh* generateFullscreenQuad()
{
	vector<GLfloat> quadVertices =
	{
		// Position        | UV
		-1.0f, 1.0f,  0.0f, 0.0f, 1.0f,
		 1.0f, 1.0f,  0.0f, 1.0f, 1.0f,
		-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
		 1.0f, -1.0f, 0.0f, 1.0f, 0.0f
	};
	vector<GLuint> quadElements = { 0, 2, 1, 1, 2, 3 };
	vector<VertexAttribute> quadLayout = {{3, GL_FLOAT}, {2, GL_FLOAT}};
	return new Mesh(quadVertices, quadElements, quadLayout);
}

Mesh* generateBox(float halfSize)
{
    return new Mesh({
		// Position						| UVs		  | Normals
        -halfSize, -halfSize, -halfSize,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f,
        halfSize, -halfSize, -halfSize,  0.0f,  0.0f, -1.0f, 1.0f, 0.0f,
        halfSize,  halfSize, -halfSize,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f,
        halfSize,  halfSize, -halfSize,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f,
        -halfSize,  halfSize, -halfSize,  0.0f,  0.0f, -1.0f, 0.0f, 1.0f,
        -halfSize, -halfSize, -halfSize,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f,

        -halfSize, -halfSize,  halfSize,  0.0f,  0.0f, 1.0f, 0.0f, 0.0f,
        halfSize, -halfSize,  halfSize,  0.0f,  0.0f, 1.0f, 1.0f, 0.0f,
        halfSize,  halfSize,  halfSize,  0.0f,  0.0f, 1.0f, 1.0f, 1.0f,
        halfSize,  halfSize,  halfSize,  0.0f,  0.0f, 1.0f, 1.0f, 1.0f,
        -halfSize,  halfSize,  halfSize,  0.0f,  0.0f, 1.0f, 0.0f, 1.0f,
        -halfSize, -halfSize,  halfSize,  0.0f,  0.0f, 1.0f, 0.0f, 0.0f,

        -halfSize,  halfSize,  halfSize, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f,
        -halfSize,  halfSize, -halfSize, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f,
        -halfSize, -halfSize, -halfSize, -1.0f,  0.0f,  0.0f, 1.0f, 1.0f,
        -halfSize, -halfSize, -halfSize, -1.0f,  0.0f,  0.0f, 1.0f, 1.0f,
        -halfSize, -halfSize,  halfSize, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f,
        -halfSize,  halfSize,  halfSize, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f,

        halfSize,  halfSize,  halfSize,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f,
        halfSize,  halfSize, -halfSize,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f,
        halfSize, -halfSize, -halfSize,  1.0f,  0.0f,  0.0f, 1.0f, 1.0f,
        halfSize, -halfSize, -halfSize,  1.0f,  0.0f,  0.0f, 1.0f, 1.0f,
        halfSize, -halfSize,  halfSize,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f,
        halfSize,  halfSize,  halfSize,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f,

        -halfSize, -halfSize, -halfSize,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f,
        halfSize, -halfSize, -halfSize,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f,
        halfSize, -halfSize,  halfSize,  0.0f, -1.0f,  0.0f, 1.0f, 1.0f,
        halfSize, -halfSize,  halfSize,  0.0f, -1.0f,  0.0f, 1.0f, 1.0f,
        -halfSize, -halfSize,  halfSize,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f,
        -halfSize, -halfSize, -halfSize,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f,

        -halfSize,  halfSize, -halfSize,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f,
        halfSize,  halfSize, -halfSize,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f,
        halfSize,  halfSize,  halfSize,  0.0f,  1.0f,  0.0f, 1.0f, 1.0f,
        halfSize,  halfSize,  halfSize,  0.0f,  1.0f,  0.0f, 1.0f, 1.0f,
        -halfSize,  halfSize,  halfSize,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f,
        -halfSize,  halfSize, -halfSize,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f
	}, {{3, GL_FLOAT}, {3, GL_FLOAT}, {2, GL_FLOAT}});
}

Mesh* generateLightSphere(float radius, int rings, int segments)
{
    vector<GLfloat> vertexData;
    vector<GLuint> indexData;

    // Generate the group of rings for the sphere
    float deltaRingAngle = (float)M_PI / rings;
    float deltaSegAngle = 2.0f * (float)M_PI / segments;
    unsigned short vertexIndex = 0;
    for (int ring = 0; ring <= rings; ring++)
    {
        float r0 = radius * sinf(ring * deltaRingAngle);
        float y0 = radius * cosf(ring * deltaRingAngle);

        // Generate the group of segments for the current ring
        for (int seg = 0; seg <= segments; seg++)
        {
            float x0 = r0 * sinf(seg * deltaSegAngle);
            float z0 = r0 * cosf(seg * deltaSegAngle);

            // Add one vertex to the strip which makes up the sphere
            vertexData.push_back(x0);
            vertexData.push_back(y0);
            vertexData.push_back(z0);

            if (ring != rings)
            {
                // each vertex (except the last) has six indices pointing to it
                indexData.push_back(vertexIndex + segments + 1);
                indexData.push_back(vertexIndex);
                indexData.push_back(vertexIndex + segments);
                indexData.push_back(vertexIndex + segments + 1);
                indexData.push_back(vertexIndex + 1);
                indexData.push_back(vertexIndex);
                vertexIndex++;
            }
        }
    }

	return new Mesh(vertexData, indexData, {{3, GL_FLOAT}});
}
