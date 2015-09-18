/*
 * GL Framework
 * Copyright (c) David Avedissian 2014-2015
 */
#include "framework/Common.h"
#include "framework/Application.h"
#include "framework/Framebuffer.h"
#include "framework/Shader.h"
#include "framework/Texture.h"
#include "framework/Mesh.h"

#define WIDTH 1024
#define HEIGHT 768

class GalaxyApp : public Application
{
private:

public:
    virtual void Startup() override
    {
    }

	virtual bool Render() override
    {
        return true;
    }

	virtual void Shutdown() override
    {
    }

	virtual void OnKeyDown(SDL_Keycode kc) override
    {
    }
};

DEFINE_MAIN_FUNCTION(GalaxyApp)
