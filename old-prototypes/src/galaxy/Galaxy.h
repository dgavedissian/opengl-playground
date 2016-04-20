/*
  Procedural Galaxy Prototype
  Copyright (c) 2013-2014 David Avedissian (avedissian.david@gmail.com)
  Author: David Avedissian
*/
#ifndef Galaxy_h__
#define Galaxy_h__

#include "../PrototypeFramework.h"
#include <random>
using std::string;

#define GALAXY_OCTREE_MAX_DEPTH 14
#define POINT_CLOUD_SIZE 100000
//#define GALAXY_DEBUG

class Galaxy;

class PointCloud
{
public:
    PointCloud(const std::string& name, const std::string& resourcegroup, const int numpoints,
               float* parray, float* carray);
    virtual ~PointCloud();

    /// Update a created pointcloud with size points.
    void updateVertexPositions(int size, float* points);
    float* lockVertexPositions();
    void unlockVertexPositions();

    /// Update vertex colours
    void updateVertexColours(int size, float* colours);

private:
    int mSize;
    Ogre::HardwareVertexBufferSharedPtr vbuf;
    Ogre::HardwareVertexBufferSharedPtr cbuf;
};

class GalaxyNode
{
public:
    GalaxyNode(const Vec3& min, const Vec3& max, uint level, Galaxy* galaxy);
    ~GalaxyNode();

    void update(const Vec3& cameraPosition, Ogre::Timer& timeTaken, float maxTime);
    void _rebuildPointCloud(float* points, uint& index);

private:
    void _updateChildren(const Vec3& cameraPosition, Ogre::Timer& timer, float maxTime);
    void _split();
    void _newChild(uint index, const Vec3& start);
    void _join();

    Vec3 mMin;
    Vec3 mMax;
    Vec3 mCentre;

    uint mLevel;
    Galaxy* mGalaxy;

    bool mHasChildren;
    GalaxyNode* mChildren[8];

    std::vector<Vec3> mStars;

#ifdef GALAXY_DEBUG
    Ogre::SceneNode* mDebugNode;
#endif
};

class Galaxy
{
public:
    Galaxy(float radius, float thickness, ulong seed);
    ~Galaxy();

    void update(const Vec3& cameraPosition);

    void _markDirty();
    float _getSplitDistanceSq(int level);
    float _sampleDensityMap(float u, float v);
    float _getDensity(const Vec3& position);

    std::mt19937& getRNG();

private:
    float mRadius;
    float mThickness;

    bool mDirty;
    float mSplitDistanceTable[GALAXY_OCTREE_MAX_DEPTH];
    PointCloud* mPointCloud;
    GalaxyNode* mRootNode;
    Ogre::Image mDensityMap;

    std::mt19937 mRNG;
};

class Application : public PrototypeFramework
{
public:
    Application();
    virtual ~Application();

protected:
    virtual void createScene();
    virtual bool frameRenderingQueued(const Ogre::FrameEvent& evt);

    Galaxy* mGalaxy;
};

#endif    // Galaxy_h__
