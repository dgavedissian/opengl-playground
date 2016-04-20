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
    virtual void startup() override
    {
        mGalaxy = new Galaxy(100.0f, 1.0f, 0xDEADBEEF);
    }

	virtual bool render() override
    {
        mGalaxy->update(glm::vec3(0, 0, 0));
        mGalaxy->render();
        return true;
    }

	virtual void shutdown() override
    {
        delete mGalaxy;
    }

	virtual void onKeyDown(SDL_Keycode) override
    {
    }
};

DEFINE_MAIN_FUNCTION(GalaxyApp)
