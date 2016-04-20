/*
  Procedural Planet Prototype
  Copyright (c) 2013-2014 David Avedissian (avedissian.david@gmail.com)
  Author: David Avedissian
*/
#pragma once

class Planet;
struct PlanetDesc;

#define RING_OCTREE_MAX_DEPTH 14

// A planetary ring system
class RingSystem
{
public:
    RingSystem(PlanetDesc& desc, Planet* parent);
    ~RingSystem();

    // Propagate updates to the detail hierarchy
    void update(const Vec3& cameraPosition);    // Camera position must be in planet space

    Ogre::ColourValue getColour(const Vec2& position) const;    // texture colour component
    float getDensity(const Vec2& position) const;               // texture alpha component

    // Accessors
    Planet* getParent();
    float getThickness() const;
    Ogre::Entity* getEntity();
    std::default_random_engine& getGenerator();
    Ogre::BillboardSet* getLargeParticleSet();
    Ogre::BillboardSet* getSmallParticleSet();

private:
    struct Asteroid
    {
        Ogre::SceneNode* node;
        Ogre::Billboard* billboard;
        float size;
        Ogre::ColourValue colour;
    };

    class Patch
    {
    public:
        Patch(const Vec2& min, const Vec2& max, uint level, RingSystem* ringSystem);
        ~Patch();

        // Check if this node needs to be joined/split
        void update(const Vec3& cameraPosition);    // Camera position must be in planet space

        // Boundaries (in planet space)
        Ogre::Vector2 mMin;
        Ogre::Vector2 mMax;
        Ogre::Vector2 mCentre;

        // Hierarchy
        uint mLevel;
        RingSystem* mParentRingSystem;

        // Child nodes
        bool mHasChildren;
        Patch* mChildren[4];

        // Asteroids
        vector<Asteroid> mAsteroidList;
        vector<Ogre::Billboard*> mDustList;

        // wat
        void _cache();

        // wat
        void _free();

        // Split a node into 4 child nodes
        void _split();

        // Join a node by deleting its children
        void _join();
    };

    float mMinRadius;
    float mMaxRadius;
    Planet* mParent;

    Ogre::MeshPtr mRingMesh;
    Ogre::MaterialPtr mRingMaterial;
    Ogre::Image mRingTexture;
    Ogre::SceneNode* mSceneNode;
    Ogre::Entity* mRingEntity;

    // Ring detail
    std::default_random_engine mRandomGenerator;
    float mDetailThresholdTable[RING_OCTREE_MAX_DEPTH];
    Patch* mDetailRootNode;
    Ogre::BillboardSet* mParticlesLarge;
    Ogre::BillboardSet* mParticlesSmall;
};
