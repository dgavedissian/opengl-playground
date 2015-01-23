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
    virtual void render();
    virtual void cleanup();

private:
    // Post process
    shared_ptr<Texture> mFbColourTex;
    shared_ptr<Framebuffer> mFb;
    shared_ptr<Shader> mPostShader;
    shared_ptr<Mesh> mQuad;

    // Mesh
    shared_ptr<Shader> mShader;
    shared_ptr<Texture> mTexture;
    shared_ptr<Mesh> mMesh;
};

#endif /* MAIN_H */
