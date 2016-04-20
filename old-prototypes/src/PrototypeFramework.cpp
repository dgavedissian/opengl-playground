/*
  Prototype Framework
  Copyright (c) 2013-2014 David Avedissian (avedissian.david@gmail.com)
  Author: David Avedissian
*/
#include "PrototypeFramework.h"

Ogre::MaterialPtr cloneMaterial(const string& originalName, const string& newName)
{
    return Ogre::MaterialManager::getSingleton().getByName(originalName)->clone(newName);
}

string generateName(const string& prefix)
{
    static std::map<string, uint> prefixCounter;
    if (prefixCounter.find(prefix) == prefixCounter.end())
        prefixCounter.insert(std::make_pair<string, uint>(string(prefix), 0));
    return prefix + Ogre::StringConverter::toString(prefixCounter[prefix]++);
}

Ogre::MeshPtr generateSphereMesh(const string& name, const float r, const int nRings,
                                 const int nSegments, bool texCoords, bool normals)
{
    using namespace Ogre;

    MeshPtr pSphere = MeshManager::getSingleton().createManual(
        name, ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
    SubMesh* pSphereVertex = pSphere->createSubMesh();

    pSphere->sharedVertexData = new VertexData();
    VertexData* vertexData = pSphere->sharedVertexData;

    // define the vertex format
    VertexDeclaration* vertexDecl = vertexData->vertexDeclaration;
    size_t currOffset = 0;
    // positions
    vertexDecl->addElement(0, currOffset, VET_FLOAT3, VES_POSITION);
    currOffset += VertexElement::getTypeSize(VET_FLOAT3);
    // normals
    vertexDecl->addElement(0, currOffset, VET_FLOAT3, VES_NORMAL);
    currOffset += VertexElement::getTypeSize(VET_FLOAT3);
    // two dimensional texture coordinates
    vertexDecl->addElement(0, currOffset, VET_FLOAT2, VES_TEXTURE_COORDINATES, 0);
    currOffset += VertexElement::getTypeSize(VET_FLOAT2);

    // allocate the vertex buffer
    vertexData->vertexCount = (nRings + 1) * (nSegments + 1);
    HardwareVertexBufferSharedPtr vBuf = HardwareBufferManager::getSingleton().createVertexBuffer(
        vertexDecl->getVertexSize(0), vertexData->vertexCount,
        HardwareBuffer::HBU_STATIC_WRITE_ONLY, false);
    VertexBufferBinding* binding = vertexData->vertexBufferBinding;
    binding->setBinding(0, vBuf);
    float* pVertex = static_cast<float*>(vBuf->lock(HardwareBuffer::HBL_DISCARD));

    // allocate index buffer
    pSphereVertex->indexData->indexCount = 6 * nRings * (nSegments + 1);
    pSphereVertex->indexData->indexBuffer = HardwareBufferManager::getSingleton().createIndexBuffer(
        HardwareIndexBuffer::IT_16BIT, pSphereVertex->indexData->indexCount,
        HardwareBuffer::HBU_STATIC_WRITE_ONLY, false);
    HardwareIndexBufferSharedPtr iBuf = pSphereVertex->indexData->indexBuffer;
    unsigned short* pIndices =
        static_cast<unsigned short*>(iBuf->lock(HardwareBuffer::HBL_DISCARD));

    float fDeltaRingAngle = (Math::PI / nRings);
    float fDeltaSegAngle = (2 * Math::PI / nSegments);
    unsigned short wVerticeIndex = 0;

    // Generate the group of rings for the sphere
    for (int ring = 0; ring <= nRings; ring++)
    {
        float r0 = r * sinf(ring * fDeltaRingAngle);
        float y0 = r * cosf(ring * fDeltaRingAngle);

        // Generate the group of segments for the current ring
        for (int seg = 0; seg <= nSegments; seg++)
        {
            float x0 = r0 * sinf(seg * fDeltaSegAngle);
            float z0 = r0 * cosf(seg * fDeltaSegAngle);

            // Add one vertex to the strip which makes up the sphere
            *pVertex++ = x0;
            *pVertex++ = y0;
            *pVertex++ = z0;

            Vector3 vNormal = Vector3(x0, y0, z0).normalisedCopy();
            *pVertex++ = vNormal.x;
            *pVertex++ = vNormal.y;
            *pVertex++ = vNormal.z;

            *pVertex++ = (float)seg / (float)nSegments;
            *pVertex++ = (float)ring / (float)nRings;

            if (ring != nRings)
            {
                // each vertex (except the last) has six indices pointing to it
                *pIndices++ = wVerticeIndex + nSegments + 1;
                *pIndices++ = wVerticeIndex;
                *pIndices++ = wVerticeIndex + nSegments;
                *pIndices++ = wVerticeIndex + nSegments + 1;
                *pIndices++ = wVerticeIndex + 1;
                *pIndices++ = wVerticeIndex;
                wVerticeIndex++;
            }
        };    // end for seg
    }         // end for ring

    // Unlock
    vBuf->unlock();
    iBuf->unlock();
    // Generate face list
    pSphereVertex->useSharedVertices = true;

    // the original code was missing this line:
    pSphere->_setBounds(AxisAlignedBox(Vector3(-r, -r, -r), Vector3(r, r, r)), false);
    pSphere->_setBoundingSphereRadius(r);
    // this line makes clear the mesh is loaded (avoids memory leaks)
    pSphere->load();

    return pSphere;
}

//-------------------------------------------------------------------------------------
PrototypeFramework::PrototypeFramework()
    : mRoot(0),
      mCamera(0),
      mActiveCamera(0),
      mSceneMgr(0),
      mWindow(0),
      mCameraControlEnabled(true),
      mCameraSpeed(10.0f),
      mCameraSpeedMultiplier(50.0f),
      mGoingForward(false),
      mGoingBack(false),
      mGoingLeft(false),
      mGoingRight(false),
      mGoingUp(false),
      mGoingDown(false),
      mFastMove(false),
      mSlowMove(false),
      mTrayMgr(0),
      mDetailsPanel(0),
      mCursorWasVisible(false),
      mShutDown(false),
      mInputManager(0),
      mMouse(0),
      mKeyboard(0),
      mOverlaySystem(0)
{
}

//-------------------------------------------------------------------------------------
PrototypeFramework::~PrototypeFramework()
{
    if (mTrayMgr)
        delete mTrayMgr;

    if (mOverlaySystem)
        delete mOverlaySystem;

    // Remove ourself as a Window listener
    Ogre::WindowEventUtilities::removeWindowEventListener(mWindow, this);
    windowClosed(mWindow);
    delete mRoot;
}

//-------------------------------------------------------------------------------------
bool PrototypeFramework::configure()
{
    // Try to restore config, if that fails then show the config dialog box
    // If the user presses cancel then return false.
    if (!mRoot->restoreConfig())
    {
        if (!mRoot->showConfigDialog())
            return false;
    }

    // Initialise Ogre
    mWindow = mRoot->initialise(true, "Prototype");
    return true;
}
//-------------------------------------------------------------------------------------
void PrototypeFramework::chooseSceneManager()
{
    // Get the SceneManager, in this case a generic one
    mSceneMgr = mRoot->createSceneManager(Ogre::ST_GENERIC);
}
//-------------------------------------------------------------------------------------
void PrototypeFramework::createCamera()
{
    // Create the camera
    mCamera = mSceneMgr->createCamera("PlayerCam");

    // Position it at 500 in Z direction
    mCamera->setPosition(Ogre::Vector3(0, 0, 80));
    // Look back along -Z
    mCamera->lookAt(Ogre::Vector3(0, 0, -300));
    mCamera->setNearClipDistance(5);

    mActiveCamera = mCamera;
}
//-------------------------------------------------------------------------------------
void PrototypeFramework::createFrameListener()
{
    Ogre::LogManager::getSingletonPtr()->logMessage("*** Initializing OIS ***");
    OIS::ParamList pl;
    size_t windowHnd = 0;
    std::ostringstream windowHndStr;

    mWindow->getCustomAttribute("WINDOW", &windowHnd);
    windowHndStr << windowHnd;
    pl.insert(std::make_pair(std::string("WINDOW"), windowHndStr.str()));

    mInputManager = OIS::InputManager::createInputSystem(pl);

    mKeyboard =
        static_cast<OIS::Keyboard*>(mInputManager->createInputObject(OIS::OISKeyboard, true));
    mMouse = static_cast<OIS::Mouse*>(mInputManager->createInputObject(OIS::OISMouse, true));

    mMouse->setEventCallback(this);
    mKeyboard->setEventCallback(this);

    // Set initial mouse clipping size
    windowResized(mWindow);

    // Register as a Window listener
    Ogre::WindowEventUtilities::addWindowEventListener(mWindow, this);

    OgreBites::InputContext input;
    input.mAccelerometer = NULL;
    input.mKeyboard = mKeyboard;
    input.mMouse = mMouse;
    input.mMultiTouch = NULL;
    mTrayMgr = new OgreBites::SdkTrayManager("InterfaceName", mWindow, input, this);
    mTrayMgr->showFrameStats(OgreBites::TL_BOTTOMLEFT);
    mTrayMgr->hideCursor();

    // create a params panel for displaying sample details
    Ogre::StringVector items;
    items.push_back("cam.pX");
    items.push_back("cam.pY");
    items.push_back("cam.pZ");
    items.push_back("");
    items.push_back("cam.oW");
    items.push_back("cam.oX");
    items.push_back("cam.oY");
    items.push_back("cam.oZ");
    items.push_back("");
    items.push_back("Filtering");
    items.push_back("Poly Mode");

    mDetailsPanel = mTrayMgr->createParamsPanel(OgreBites::TL_NONE, "DetailsPanel", 200, items);
    mDetailsPanel->setParamValue(9, "Bilinear");
    mDetailsPanel->setParamValue(10, "Solid");
    mDetailsPanel->hide();

    mRoot->addFrameListener(this);
    mSceneMgr->addRenderQueueListener(mOverlaySystem);
}
//-------------------------------------------------------------------------------------
void PrototypeFramework::destroyScene()
{
    mSceneMgr->removeRenderQueueListener(mOverlaySystem);
}
//-------------------------------------------------------------------------------------
void PrototypeFramework::createViewports()
{
    // Create one viewport, entire window
    Ogre::Viewport* vp = mWindow->addViewport(mCamera);
    vp->setBackgroundColour(Ogre::ColourValue(0, 0, 0));

    // Alter the camera aspect ratio to match the viewport
    mCamera->setAspectRatio(Ogre::Real(vp->getActualWidth()) / Ogre::Real(vp->getActualHeight()));
}
//-------------------------------------------------------------------------------------
void PrototypeFramework::setupResources()
{
    // Load resource paths from config file
    Ogre::ConfigFile cf;
    cf.load("resources.cfg");

    // Go through all sections & settings in the file
    Ogre::ConfigFile::SectionIterator seci = cf.getSectionIterator();

    Ogre::String secName, typeName, archName;

    while (seci.hasMoreElements())
    {
        secName = seci.peekNextKey();
        Ogre::ConfigFile::SettingsMultiMap* settings = seci.getNext();
        Ogre::ConfigFile::SettingsMultiMap::iterator i;

        for (i = settings->begin(); i != settings->end(); ++i)
        {
            typeName = i->first;
            archName = i->second;
            Ogre::ResourceGroupManager::getSingleton().addResourceLocation(archName, typeName,
                                                                           secName);
        }
    }
}
//-------------------------------------------------------------------------------------
void PrototypeFramework::createResourceListener()
{
}
//-------------------------------------------------------------------------------------
void PrototypeFramework::loadResources()
{
    Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();
}
//-------------------------------------------------------------------------------------
void PrototypeFramework::go()
{
    if (!setup())
        return;

    mRoot->startRendering();

    // clean up
    destroyScene();
}
//-------------------------------------------------------------------------------------
bool PrototypeFramework::setup()
{
#ifdef _DEBUG
    mRoot = new Ogre::Root("plugins_d.cfg");
#else
    mRoot = new Ogre::Root("plugins.cfg");
#endif
    mOverlaySystem = new Ogre::OverlaySystem();
    setupResources();

    if (!configure())
        return false;

    chooseSceneManager();
    createCamera();
    createViewports();

    // Set default mipmap level (NB some APIs ignore this)
    Ogre::TextureManager::getSingleton().setDefaultNumMipmaps(5);

    // Create any resource listeners (for loading screens)
    createResourceListener();
    // Load resources
    loadResources();

    // Create the scene
    createScene();

    createFrameListener();

    return true;
};
//-------------------------------------------------------------------------------------
bool PrototypeFramework::frameRenderingQueued(const Ogre::FrameEvent& evt)
{
    if (mWindow->isClosed())
        return false;

    if (mShutDown)
        return false;

    mMouse->capture();
    mKeyboard->capture();

    mTrayMgr->frameRenderingQueued(evt);

    if (!mTrayMgr->isDialogVisible() && mCameraControlEnabled)
    {
        // Control the camera
        Ogre::Vector3 velocity = Ogre::Vector3::ZERO;
        if (mGoingForward)
            velocity += mCamera->getDirection();
        if (mGoingBack)
            velocity -= mCamera->getDirection();
        if (mGoingRight)
            velocity += mCamera->getRight();
        if (mGoingLeft)
            velocity -= mCamera->getRight();
        if (mGoingUp)
            velocity += mCamera->getUp();
        if (mGoingDown)
            velocity -= mCamera->getUp();

        // Apply velocity speed to the direction vector
        float speed;
        if (mFastMove)
            speed = mSlowMove ? mCameraSpeed : mCameraSpeed * mCameraSpeedMultiplier;
        else if (mSlowMove)
            speed = mCameraSpeed / mCameraSpeedMultiplier;
        else
            speed = mCameraSpeed;
        if (velocity.squaredLength() > 0.0f)
        {
            velocity.normalise();
            velocity *= speed;
        }

        // Move the camera
        if (velocity != Ogre::Vector3::ZERO)
            mCamera->move(velocity * evt.timeSinceLastFrame);

        if (mDetailsPanel->isVisible())    // if details panel is visible, then update
                                           // its contents
        {
            mDetailsPanel->setParamValue(
                0, Ogre::StringConverter::toString(mActiveCamera->getDerivedPosition().x));
            mDetailsPanel->setParamValue(
                1, Ogre::StringConverter::toString(mActiveCamera->getDerivedPosition().y));
            mDetailsPanel->setParamValue(
                2, Ogre::StringConverter::toString(mActiveCamera->getDerivedPosition().z));
            mDetailsPanel->setParamValue(
                4, Ogre::StringConverter::toString(mActiveCamera->getDerivedOrientation().w));
            mDetailsPanel->setParamValue(
                5, Ogre::StringConverter::toString(mActiveCamera->getDerivedOrientation().x));
            mDetailsPanel->setParamValue(
                6, Ogre::StringConverter::toString(mActiveCamera->getDerivedOrientation().y));
            mDetailsPanel->setParamValue(
                7, Ogre::StringConverter::toString(mActiveCamera->getDerivedOrientation().z));
        }
    }

    return true;
}
//-------------------------------------------------------------------------------------
bool PrototypeFramework::keyPressed(const OIS::KeyEvent& arg)
{
    if (mTrayMgr->isDialogVisible())
        return true;    // don't process any more keys if dialog is up

    if (arg.key == OIS::KC_F)    // toggle visibility of advanced frame stats
    {
        mTrayMgr->toggleAdvancedFrameStats();
    }
    else if (arg.key == OIS::KC_G)    // toggle visibility of even rarer debugging details
    {
        if (mDetailsPanel->getTrayLocation() == OgreBites::TL_NONE)
        {
            mTrayMgr->moveWidgetToTray(mDetailsPanel, OgreBites::TL_TOPRIGHT, 0);
            mDetailsPanel->show();
        }
        else
        {
            mTrayMgr->removeWidgetFromTray(mDetailsPanel);
            mDetailsPanel->hide();
        }
    }
    else if (arg.key == OIS::KC_T)    // cycle polygon rendering mode
    {
        Ogre::String newVal;
        Ogre::TextureFilterOptions tfo;
        unsigned int aniso;

        switch (mDetailsPanel->getParamValue(9).asUTF8()[0])
        {
        case 'B':
            newVal = "Trilinear";
            tfo = Ogre::TFO_TRILINEAR;
            aniso = 1;
            break;

        case 'T':
            newVal = "Anisotropic";
            tfo = Ogre::TFO_ANISOTROPIC;
            aniso = 8;
            break;

        case 'A':
            newVal = "None";
            tfo = Ogre::TFO_NONE;
            aniso = 1;
            break;

        default:
            newVal = "Bilinear";
            tfo = Ogre::TFO_BILINEAR;
            aniso = 1;
        }

        Ogre::MaterialManager::getSingleton().setDefaultTextureFiltering(tfo);
        Ogre::MaterialManager::getSingleton().setDefaultAnisotropy(aniso);
        mDetailsPanel->setParamValue(9, newVal);
    }
    else if (arg.key == OIS::KC_R)    // cycle polygon rendering mode
    {
        Ogre::String newVal;
        Ogre::PolygonMode pm;

        switch (mActiveCamera->getPolygonMode())
        {
        case Ogre::PM_SOLID:
            newVal = "Wireframe";
            pm = Ogre::PM_WIREFRAME;
            break;

        case Ogre::PM_WIREFRAME:
            newVal = "Points";
            pm = Ogre::PM_POINTS;
            break;

        default:
            newVal = "Solid";
            pm = Ogre::PM_SOLID;
        }

        mActiveCamera->setPolygonMode(pm);
        mDetailsPanel->setParamValue(10, newVal);
    }
    else if (arg.key == OIS::KC_F5)    // refresh all textures
    {
        Ogre::TextureManager::getSingleton().reloadAll();
    }
    else if (arg.key == OIS::KC_SYSRQ)    // take a screenshot
    {
        mWindow->writeContentsToTimestampedFile("screenshot", ".jpg");
    }
    else if (arg.key == OIS::KC_ESCAPE)
    {
        mShutDown = true;
    }

    // Camera Control
    if (arg.key == OIS::KC_W || arg.key == OIS::KC_UP)
        mGoingForward = true;
    else if (arg.key == OIS::KC_S || arg.key == OIS::KC_DOWN)
        mGoingBack = true;
    else if (arg.key == OIS::KC_A || arg.key == OIS::KC_LEFT)
        mGoingLeft = true;
    else if (arg.key == OIS::KC_D || arg.key == OIS::KC_RIGHT)
        mGoingRight = true;
    else if (arg.key == OIS::KC_PGUP)
        mGoingUp = true;
    else if (arg.key == OIS::KC_PGDOWN)
        mGoingDown = true;
    else if (arg.key == OIS::KC_LSHIFT)
        mFastMove = true;
    else if (arg.key == OIS::KC_LCONTROL)
        mSlowMove = true;

    return true;
}

bool PrototypeFramework::keyReleased(const OIS::KeyEvent& arg)
{
    if (mTrayMgr->isDialogVisible())
        return true;    // don't process any more keys if dialog is up

    // Camera Control
    if (arg.key == OIS::KC_W || arg.key == OIS::KC_UP)
        mGoingForward = false;
    else if (arg.key == OIS::KC_S || arg.key == OIS::KC_DOWN)
        mGoingBack = false;
    else if (arg.key == OIS::KC_A || arg.key == OIS::KC_LEFT)
        mGoingLeft = false;
    else if (arg.key == OIS::KC_D || arg.key == OIS::KC_RIGHT)
        mGoingRight = false;
    else if (arg.key == OIS::KC_PGUP)
        mGoingUp = false;
    else if (arg.key == OIS::KC_PGDOWN)
        mGoingDown = false;
    else if (arg.key == OIS::KC_LSHIFT)
        mFastMove = false;
    else if (arg.key == OIS::KC_LCONTROL)
        mSlowMove = false;

    return true;
}

bool PrototypeFramework::mouseMoved(const OIS::MouseEvent& arg)
{
    mTrayMgr->injectMouseMove(arg);

    if (mTrayMgr->isDialogVisible())
        return true;    // don't process any more keys if dialog is up

    /*
    mCamera->yaw(Ogre::Degree(-arg.state.X.rel * 0.15f));
    mCamera->pitch(Ogre::Degree(-arg.state.Y.rel * 0.15f));
    */

    mCamera->setOrientation(mCamera->getOrientation() *
                            Quat(Ogre::Degree(-arg.state.X.rel * 0.15f), Vec3::UNIT_Y) *
                            Quat(Ogre::Degree(-arg.state.Y.rel * 0.15f), Vec3::UNIT_X));
    return true;
}

bool PrototypeFramework::mousePressed(const OIS::MouseEvent& arg, OIS::MouseButtonID id)
{
    mTrayMgr->injectMouseDown(arg, id);

    if (mTrayMgr->isDialogVisible())
        return true;    // don't process any more keys if dialog is up

    return true;
}

bool PrototypeFramework::mouseReleased(const OIS::MouseEvent& arg, OIS::MouseButtonID id)
{
    mTrayMgr->injectMouseUp(arg, id);

    if (mTrayMgr->isDialogVisible())
        return true;    // don't process any more keys if dialog is up

    return true;
}

// Adjust mouse clipping area
void PrototypeFramework::windowResized(Ogre::RenderWindow* rw)
{
    unsigned int width, height, depth;
    int left, top;
    rw->getMetrics(width, height, depth, left, top);

    const OIS::MouseState& ms = mMouse->getMouseState();
    ms.width = width;
    ms.height = height;
}

// Unattach OIS before window shutdown (very important under Linux)
void PrototypeFramework::windowClosed(Ogre::RenderWindow* rw)
{
    // Only close for window that created OIS (the main window in these demos)
    if (rw == mWindow)
    {
        if (mInputManager)
        {
            mInputManager->destroyInputObject(mMouse);
            mInputManager->destroyInputObject(mKeyboard);

            OIS::InputManager::destroyInputSystem(mInputManager);
            mInputManager = 0;
        }
    }
}