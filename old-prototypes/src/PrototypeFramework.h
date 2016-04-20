/*
  Prototype Framework
  Copyright (c) 2013-2014 David Avedissian (avedissian.david@gmail.com)
  Author: David Avedissian
*/
#pragma once

// C headers
#include <cassert>

// C++ Headers

// Ogre
#include <Ogre.h>
#include <OIS.h>
#include <SdkTrays.h>
#include <SdkCameraMan.h>

using std::string;
typedef unsigned int uint;
typedef unsigned long ulong;
typedef Ogre::Vector2 Vec2;
typedef Ogre::Vector3 Vec3;
typedef Ogre::Vector4 Vec4;
typedef Ogre::Quaternion Quat;

// Utilities
Ogre::MaterialPtr cloneMaterial(const string& originalName, const string& newName);
string generateName(const string& prefix);
Ogre::MeshPtr generateSphereMesh(const string& name, const float r, const int nRings = 16,
                                 const int nSegments = 16, bool texCoords = true,
                                 bool normals = true);

class PrototypeFramework : public Ogre::FrameListener,
                           public Ogre::WindowEventListener,
                           public OIS::KeyListener,
                           public OIS::MouseListener,
                           OgreBites::SdkTrayListener
{
public:
    PrototypeFramework();
    virtual ~PrototypeFramework();

    virtual void go();

protected:
    virtual bool setup();
    virtual bool configure();
    virtual void chooseSceneManager();
    virtual void createCamera();
    virtual void createFrameListener();
    virtual void createScene() = 0;    // Override me!
    virtual void destroyScene();
    virtual void createViewports();
    virtual void setupResources();
    virtual void createResourceListener();
    virtual void loadResources();
    virtual bool frameRenderingQueued(const Ogre::FrameEvent& evt);

    virtual bool keyPressed(const OIS::KeyEvent& arg);
    virtual bool keyReleased(const OIS::KeyEvent& arg);
    virtual bool mouseMoved(const OIS::MouseEvent& arg);
    virtual bool mousePressed(const OIS::MouseEvent& arg, OIS::MouseButtonID id);
    virtual bool mouseReleased(const OIS::MouseEvent& arg, OIS::MouseButtonID id);

    // Adjust mouse clipping area
    virtual void windowResized(Ogre::RenderWindow* rw);
    // Unattach OIS before window shutdown (very important under Linux)
    virtual void windowClosed(Ogre::RenderWindow* rw);

    Ogre::Root* mRoot;
    Ogre::Camera* mCamera;
    Ogre::Camera* mActiveCamera;
    Ogre::SceneManager* mSceneMgr;
    Ogre::RenderWindow* mWindow;

    // Camera Control
    bool mCameraControlEnabled;
    float mCameraSpeed;
    float mCameraSpeedMultiplier;
    bool mGoingForward;
    bool mGoingBack;
    bool mGoingLeft;
    bool mGoingRight;
    bool mGoingUp;
    bool mGoingDown;
    bool mFastMove;
    bool mSlowMove;

    // OgreBites
    OgreBites::SdkTrayManager* mTrayMgr;
    OgreBites::ParamsPanel* mDetailsPanel;    // sample details panel
    bool mCursorWasVisible;                   // was cursor visible before dialog appeared
    bool mShutDown;
    Ogre::OverlaySystem* mOverlaySystem;    // Overlay system

    // OIS Input devices
    OIS::InputManager* mInputManager;
    OIS::Mouse* mMouse;
    OIS::Keyboard* mKeyboard;
};
