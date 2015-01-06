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

  // Vertices
  static const float vertices[] = {
    -1.0f, -1.0f, 0.0f,
    1.0f, -1.0f, 0.0f,
    0.0f, 1.0f, 0.0f
  };

  // Generate vertex buffer
  glGenBuffers(1, &mVB);
  glBindBuffer(GL_ARRAY_BUFFER, mVB);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW); 
}

void DeferredShading::render()
{
  // Draw vertex buffer
  glEnableVertexAttribArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, mVB);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
  glUseProgram(mShader);
  glDrawArrays(GL_TRIANGLES, 0, 3);
  glDisableVertexAttribArray(0);
}

void DeferredShading::cleanup()
{
  glDeleteProgram(mShader);
}

