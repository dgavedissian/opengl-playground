/*
  Procedural Planet Prototype
  Copyright (c) 2013-2014 David Avedissian (avedissian.david@gmail.com)
  Author: David Avedissian
*/
#include "Main.h"
#include "Planet.h"

Vec3 lightDirection;
Ogre::SceneManager* sceneMgr = nullptr;

Application::Application()
{
}

Application::~Application()
{
    mPlanet.reset();
}

void Application::createScene()
{
    float radius = 6300000.0f;
    lightDirection = Ogre::Vector3(1.0f, 0.0f, 0.0f).normalisedCopy();

    sceneMgr = mSceneMgr;
    mSceneMgr->setAmbientLight(Ogre::ColourValue(0.0f, 0.0f, 0.0f, 1.0f));
    mCamera->setPosition(Ogre::Vector3(0.0f, radius * 0.2f, radius * 2.0f));
    mCamera->setFOVy(Ogre::Radian(3.14159f / 3.0f));
    mCamera->setNearClipDistance(1.0f);
    mCamera->setFarClipDistance(radius * 10.0f);

    PlanetDesc desc;
    desc.axialTilt = 0.0f;
    desc.radius = radius;
    desc.rotationPeriod = 1.0f;
    desc.surfaceTexture = "planet.jpg";
    desc.hasAtmosphere = false;
    desc.atmosphere.radius = desc.radius * 1.025f;
    desc.hasRings = false;
    desc.rings.minRadius = desc.radius * 1.5f;
    desc.rings.maxRadius = desc.radius * 2.5f;
    desc.rings.texture = "rings.png";
    mPlanet.reset(new Planet(desc));

    Ogre::Light* light = sceneMgr->createLight();
    light->setType(Ogre::Light::LT_DIRECTIONAL);
    light->setDirection(lightDirection);
}

bool Application::frameRenderingQueued(const Ogre::FrameEvent& evt)
{
    mPlanet->update(evt.timeSinceLastFrame, mCamera->getPosition());

    // Calculate new camera speed
    // Speed ~ Distance from surface
    float height =
        mPlanet->getPosition().distance(mCamera->getPosition()) - mPlanet->getDesc().radius;
    mCameraSpeed = height * 0.05f;

    return PrototypeFramework::frameRenderingQueued(evt);
}

// Start Up
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#endif

#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
INT WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR strCmdLine, INT)
#else
int main(int argc, char* argv[])
#endif
{
    // Create application object
    Application app;

    try
    {
        app.go();
    }
    catch (Ogre::Exception& e)
    {
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
        MessageBox(NULL, e.getFullDescription().c_str(), "An exception has occured!",
                   MB_OK | MB_ICONERROR | MB_TASKMODAL);
#else
        std::cerr << "An exception has occured: " << e.getFullDescription().c_str() << std::endl;
#endif
    }

    return 0;
}
