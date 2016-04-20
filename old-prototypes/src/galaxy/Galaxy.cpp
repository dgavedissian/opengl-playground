/*
  Procedural Galaxy Prototype
  Copyright (c) 2013-2014 David Avedissian (avedissian.david@gmail.com)
  Author: David Avedissian
*/
#include "Galaxy.h"

Ogre::SceneManager* sceneMgr;

float lerp(float a, float b, float t)
{
    return a + (b - a) * t;
}

PointCloud::PointCloud(const std::string& name, const std::string& resourcegroup,
                       const int numpoints, float* parray, float* carray)
{
    /// Create the mesh via the MeshManager
    Ogre::MeshPtr msh = Ogre::MeshManager::getSingleton().createManual(name, resourcegroup);

    /// Create one submesh
    Ogre::SubMesh* sub = msh->createSubMesh();

    /// Create vertex data structure for vertices shared between submeshes
    msh->sharedVertexData = new Ogre::VertexData();
    msh->sharedVertexData->vertexCount = numpoints;

    /// Create declaration (memory format) of vertex data
    Ogre::VertexDeclaration* decl = msh->sharedVertexData->vertexDeclaration;
    decl->addElement(0, 0, Ogre::VET_FLOAT3, Ogre::VES_POSITION);

    vbuf = Ogre::HardwareBufferManager::getSingleton().createVertexBuffer(
        decl->getVertexSize(0), msh->sharedVertexData->vertexCount,
        Ogre::HardwareBuffer::HBU_DYNAMIC_WRITE_ONLY_DISCARDABLE);

    /// Upload the vertex data to the card
    vbuf->writeData(0, vbuf->getSizeInBytes(), parray, true);

    if (carray != NULL)
    {
        // Create 2nd buffer for colors
        decl->addElement(1, 0, Ogre::VET_COLOUR, Ogre::VES_DIFFUSE);

        cbuf = Ogre::HardwareBufferManager::getSingleton().createVertexBuffer(
            Ogre::VertexElement::getTypeSize(Ogre::VET_COLOUR), msh->sharedVertexData->vertexCount,
            Ogre::HardwareBuffer::HBU_DYNAMIC_WRITE_ONLY_DISCARDABLE);

        Ogre::RenderSystem* rs = Ogre::Root::getSingleton().getRenderSystem();
        Ogre::RGBA* colours = new Ogre::RGBA[numpoints];
        for (int i = 0, k = 0; i < numpoints * 3, k < numpoints; i += 3, k++)
        {
            // Use render system to convert colour value since colour packing varies
            rs->convertColourValue(Ogre::ColourValue(carray[i], carray[i + 1], carray[i + 2]),
                                   &colours[k]);
        }

        // Upload colour data
        cbuf->writeData(0, cbuf->getSizeInBytes(), colours, true);
        delete[] colours;
    }

    /// Set vertex buffer binding so buffer 0 is bound to our vertex buffer
    Ogre::VertexBufferBinding* bind = msh->sharedVertexData->vertexBufferBinding;
    bind->setBinding(0, vbuf);

    if (carray != NULL)
    {
        // Set colour binding so buffer 1 is bound to colour buffer
        bind->setBinding(1, cbuf);
    }

    sub->useSharedVertices = true;
    sub->operationType = Ogre::RenderOperation::OT_POINT_LIST;

    msh->load();
    msh->_setBoundingSphereRadius(100000000000.0f);
    msh->_setBounds(Ogre::AxisAlignedBox(-Vec3(1000000000.0f), Vec3(10000000000.0f)));
}

PointCloud::~PointCloud()
{
}

void PointCloud::updateVertexPositions(int size, float* points)
{
    float* pPArray = lockVertexPositions();
    for (int i = 0; i < size * 3; i += 3)
    {
        pPArray[i] = points[i];
        pPArray[i + 1] = points[i + 1];
        pPArray[i + 2] = points[i + 2];
    }
    unlockVertexPositions();
}

float* PointCloud::lockVertexPositions()
{
    return static_cast<float*>(vbuf->lock(Ogre::HardwareBuffer::HBL_DISCARD));
}

void PointCloud::unlockVertexPositions()
{
    vbuf->unlock();
}

void PointCloud::updateVertexColours(int size, float* colours)
{
    float* pCArray = static_cast<float*>(cbuf->lock(Ogre::HardwareBuffer::HBL_DISCARD));
    for (int i = 0; i < size * 3; i += 3)
    {
        pCArray[i] = colours[i];
        pCArray[i + 1] = colours[i + 1];
        pCArray[i + 2] = colours[i + 2];
    }
    cbuf->unlock();
}

GalaxyNode::GalaxyNode(const Vec3& min, const Vec3& max, uint level, Galaxy* galaxy)
    : mMin(min),
      mMax(max),
      mCentre((min + max) * 0.5f),
      mLevel(level),
      mGalaxy(galaxy),
      mHasChildren(false)
{
    std::uniform_real_distribution<float> probabilityDistribution(0.0f, 1.0f);
    std::uniform_real_distribution<float> positionDistributionX(min.x, max.x);
    std::uniform_real_distribution<float> positionDistributionY(min.y, max.y);
    std::uniform_real_distribution<float> positionDistributionZ(min.z, max.z);

    // Generate stars
    uint starCount = std::max(2000000 / (int)pow(8.0f, (int)mLevel), 10);
    for (uint i = 0; i < starCount; ++i)
    {
        Vec3 position(positionDistributionX(galaxy->getRNG()),
                      positionDistributionY(galaxy->getRNG()),
                      positionDistributionZ(galaxy->getRNG()));

        // Only spawn if a random number is > the density function at the position
        if (mGalaxy->_getDensity(position) > probabilityDistribution(galaxy->getRNG()))
            mStars.push_back(position);
    }

#ifdef GALAXY_DEBUG
    Ogre::Entity* box = sceneMgr->createEntity(Ogre::SceneManager::PT_CUBE);
    box->setMaterialName("Galaxy/Debug/OctreeBox");
    mDebugNode = sceneMgr->getRootSceneNode()->createChildSceneNode(mCentre);
    mDebugNode->attachObject(box);
    mDebugNode->setScale((mMax - mMin) / 100.0f);
#endif
}

GalaxyNode::~GalaxyNode()
{
    mStars.clear();

#ifdef GALAXY_DEBUG
    sceneMgr->destroyEntity((Ogre::Entity*)mDebugNode->getAttachedObject(0));
    sceneMgr->destroySceneNode(mDebugNode);
#endif

    if (mHasChildren)
        _join();
}

void GalaxyNode::update(const Vec3& cameraPosition, Ogre::Timer& timer, float maxTime)
{
    // Return immediately if we've run out of time
    if (timer.getMilliseconds() > (maxTime * 1000.0f))
        return;

    float distance = cameraPosition.squaredDistance(mCentre);

    if (mHasChildren)
    {
        // Combine the node if it has children if the distance is too far.
        if (distance > mGalaxy->_getSplitDistanceSq(mLevel))
        {
            _join();
        }
        else
        {
            // Otherwise update...
            _updateChildren(cameraPosition, timer, maxTime);
        }
    }
    else
    {
        // If we have no stars at all, no point subdividing
        if (mStars.size() > 0)
        {
            // Split the node into 4 children if the distance threshold is reached
            if (distance < mGalaxy->_getSplitDistanceSq(mLevel) &&
                mLevel < (GALAXY_OCTREE_MAX_DEPTH - 1))
            {
                _split();
                _updateChildren(cameraPosition, timer, maxTime);
            }
        }
    }
}

void GalaxyNode::_rebuildPointCloud(float* points, uint& index)
{
    if (index >= POINT_CLOUD_SIZE)
        return;

    // Add stars
    for (uint i = 0; i < mStars.size(); ++i)
    {
        Vec3& star = mStars[i];
        points[index * 3] = star.x;
        points[index * 3 + 1] = star.y;
        points[index * 3 + 2] = star.z;
        index++;
    }

    // Recurse into children
    if (mHasChildren)
    {
        for (uint i = 0; i < 8; ++i)
            mChildren[i]->_rebuildPointCloud(points, index);
    }
}

void GalaxyNode::_updateChildren(const Vec3& cameraPosition, Ogre::Timer& timer, float maxTime)
{
    assert(mHasChildren);
    for (uint i = 0; i < 8; ++i)
        mChildren[i]->update(cameraPosition, timer, maxTime);
}

void GalaxyNode::_split()
{
    Vec3 half = 0.5f * (mMax - mMin);
    _newChild(0, mMin + Vec3(0.0f, half.y, 0.0f));
    _newChild(1, mMin + Vec3(half.x, half.y, 0.0f));
    _newChild(2, mMin);
    _newChild(3, mMin + Vec3(half.x, 0.0f, 0.0f));
    _newChild(4, mMin + Vec3(0.0f, half.y, half.z));
    _newChild(5, mMin + Vec3(half.x, half.y, half.z));
    _newChild(6, mMin + Vec3(0.0f, 0.0f, half.z));
    _newChild(7, mMin + Vec3(half.x, 0.0f, half.z));
    mHasChildren = true;
    mGalaxy->_markDirty();
}

void GalaxyNode::_newChild(uint index, const Vec3& start)
{
    assert(index >= 0 && index < 8);
    mChildren[index] = new GalaxyNode(start, start + 0.5f * (mMax - mMin), mLevel + 1, mGalaxy);
}

void GalaxyNode::_join()
{
    for (uint i = 0; i < 8; ++i)
        delete mChildren[i];
    mHasChildren = false;
    mGalaxy->_markDirty();
}

Galaxy::Galaxy(float radius, float thickness, ulong seed)
    : mRadius(radius),
      mThickness(thickness),
      mDirty(false),
      mPointCloud(nullptr),
      mRootNode(nullptr)
{
    mRNG.seed(seed);

    // Compute the level of detail distances
    float start = radius * 4.0f;
    mSplitDistanceTable[0] = pow(start, 2);
    for (int i = 1; i < GALAXY_OCTREE_MAX_DEPTH; i++)
        mSplitDistanceTable[i] = mSplitDistanceTable[i - 1] * 0.25f;

    // Load the ring texture (used for density lookups).
    mDensityMap.load("sbb-density.jpg", "General");

    // Create the root node
    mRootNode = new GalaxyNode(Vec3(-radius), Vec3(radius), 0, this);

    // Build the point cloud
    float* points = new float[POINT_CLOUD_SIZE * 3];
    uint indexCounter = 0;
    mRootNode->_rebuildPointCloud(points, indexCounter);
    for (uint i = indexCounter * 3; i != POINT_CLOUD_SIZE * 3; i++)
        points[i] = 0;
    mPointCloud =
        new PointCloud("PointCloud", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
                       POINT_CLOUD_SIZE, points, 0);
    delete[] points;

    Ogre::Entity* pcEnt = sceneMgr->createEntity("PointCloud");
    pcEnt->setMaterialName("Galaxy/Debug/Point");
    sceneMgr->getRootSceneNode()->attachObject(pcEnt);
}

Galaxy::~Galaxy()
{
    delete mPointCloud;
    delete mRootNode;
}

void Galaxy::update(const Vec3& cameraPosition)
{
    Ogre::Timer timer;
    timer.reset();
    mRootNode->update(cameraPosition, timer, 1.0f / 30.0f);

    // Update the point cloud
    // TODO reuse points buffer
    if (mDirty)
    {
        float* points = mPointCloud->lockVertexPositions();
        uint indexCounter = 0;
        mRootNode->_rebuildPointCloud(points, indexCounter);
        for (uint i = indexCounter * 3; i < POINT_CLOUD_SIZE * 3; i++)
            points[i] = 0;
        mPointCloud->unlockVertexPositions();
        mDirty = false;
    }
}

void Galaxy::_markDirty()
{
    mDirty = true;
}

float Galaxy::_getSplitDistanceSq(int level)
{
    assert(level < GALAXY_OCTREE_MAX_DEPTH);
    return mSplitDistanceTable[level];
}

float Galaxy::_sampleDensityMap(float u, float v)
{
    // Use Bilinear filtering to generate an interpolated pixel
    u = u * mDensityMap.getWidth();
    v = v * mDensityMap.getHeight();
    size_t x = floor(u);
    size_t y = floor(v);
    float u_ratio = u - (float)x;
    float v_ratio = v - (float)y;
    float u_opposite = 1.0f - u_ratio;
    float v_opposite = 1.0f - v_ratio;
    return (mDensityMap.getColourAt(x, y, 0).r * u_opposite +
            mDensityMap.getColourAt(x + 1, y, 0).r * u_ratio) *
               v_opposite +
           (mDensityMap.getColourAt(x, y + 1, 0).r * u_opposite +
            mDensityMap.getColourAt(x + 1, y + 1, 0).r * u_ratio) *
               v_ratio;
}

float Galaxy::_getDensity(const Vec3& position)
{
    // Calculate vertical density
    float relativeHeight = abs(position.y) / mRadius;
    float ratio = relativeHeight * (mRadius / mThickness);
    float verticalDensity = exp(-ratio * ratio);

    // Lookup the density from the texture
    Vec2 tc(position.x / (2.0f * mRadius) + 0.5f, position.z / (2.0f * mRadius) + 0.5f);
    float planeDensity = _sampleDensityMap(tc.x, tc.y) * verticalDensity;

    // Generate the galactic core
    float gcXRadius = pow(mRadius * 0.25f, 2);
    float gcYRadius = pow(mRadius * 0.1f, 2);
    float gcZRadius = gcXRadius;
    float galacticCoreDensity = std::max(
        0.0f, 1.0f - (position.x * position.x / gcXRadius + position.z * position.z / gcZRadius +
                      position.y * position.y / gcYRadius));
    galacticCoreDensity *= galacticCoreDensity;

    return planeDensity + galacticCoreDensity;
}

std::mt19937& Galaxy::getRNG()
{
    return mRNG;
}

Application::Application() : mGalaxy(nullptr)
{
}

Application::~Application()
{
    if (mGalaxy)
        delete mGalaxy;
}

void Application::createScene()
{
    float radius = 100000.0f;

    sceneMgr = mSceneMgr;
    mSceneMgr->setAmbientLight(Ogre::ColourValue(0.0f, 0.0f, 0.0f, 1.0f));
    mCamera->setPosition(Ogre::Vector3(0.0f, radius * 0.2f, radius * 1.2f));
    mCamera->setFOVy(Ogre::Radian(Ogre::Math::PI / 3.0f));
    mCamera->setFarClipDistance(1000000.0f);

    mGalaxy = new Galaxy(radius, radius / 100.0f, 2196);

    // Make the camera man faster
    mCameraSpeed = 500.0f;
}

bool Application::frameRenderingQueued(const Ogre::FrameEvent& evt)
{
    mGalaxy->update(mCamera->getPosition());
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
