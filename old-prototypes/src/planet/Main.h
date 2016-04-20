/*
  Procedural Planet Prototype
  Copyright (c) 2013-2014 David Avedissian (avedissian.david@gmail.com)
  Author: David Avedissian
*/
#pragma once

#include "../PrototypeFramework.h"

#include <memory>
#include <random>
#include <stack>
using std::string;
using std::vector;
using std::shared_ptr;
using std::weak_ptr;
using std::dynamic_pointer_cast;
using std::enable_shared_from_this;

#include "simplexnoise.h"

class Planet;

class Application : public PrototypeFramework
{
public:
    Application();
    virtual ~Application();

protected:
    virtual void createScene();
    virtual bool frameRenderingQueued(const Ogre::FrameEvent& evt);

    shared_ptr<Planet> mPlanet;
};

extern Vec3 lightDirection;
extern Ogre::SceneManager* sceneMgr;
