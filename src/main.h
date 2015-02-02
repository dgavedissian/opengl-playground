/*
 * GL Framework
 * Copyright (c) David Avedissian 2014-2015
 */
#ifndef MAIN_H
#define MAIN_H

class DeferredShading : public Framework
{
public:
    virtual void setup();
    virtual bool drawFrame();
    virtual void cleanup();
    virtual void onKeyDown(SDL_Keycode kc);

private:
    // Post process
    Framebuffer* mFb;
    VertexBuffer* mQuad;
    Shader* mPostShader;

    // Mesh
    VertexBuffer* mMesh;
    Texture* mTexture;
    Shader* mShader;
};

#endif /* MAIN_H */
