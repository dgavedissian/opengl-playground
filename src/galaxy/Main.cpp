/*
 * Galaxy Prototype
 * Copyright (c) David Avedissian 2014-2015
 */
#include "framework/Common.h"
#include "framework/Application.h"
#include "Galaxy.h"

#define WIDTH 1024
#define HEIGHT 768

class GalaxyApp : public Application
{
private:
    Galaxy* mGalaxy;

public:
    virtual void Startup() override
    {
        mGalaxy = new Galaxy(100.0f, 1.0f, 0xDEADBEEF);
    }

	virtual bool Render() override
    {
        mGalaxy->Update(glm::vec3(0, 0, 0));
        mGalaxy->Render();
        return true;
    }

	virtual void Shutdown() override
    {
        delete mGalaxy;
    }

	virtual void OnKeyDown(SDL_Keycode) override
    {
    }
};

DEFINE_MAIN_FUNCTION(GalaxyApp)
