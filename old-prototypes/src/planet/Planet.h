/*
  Procedural Planet Prototype
  Copyright (c) 2013-2014 David Avedissian (avedissian.david@gmail.com)
  Author: David Avedissian
*/
#pragma once

// Forward declarations
class RingSystem;
class Planet;
struct PlanetDesc;

// Other includes
#include "RingSystem.h"
#include "Terrain.h"

// Contains all the information needed to construct a planetary object
struct PlanetDesc
{
    float radius;
    float rotationPeriod;    // in seconds
    float axialTilt;         // in radians
    string surfaceTexture;
    string nightTexture;

    bool hasAtmosphere;
    struct
    {
        float radius;
    } atmosphere;

    bool hasRings;
    struct
    {
        float minRadius;
        float maxRadius;
        string texture;
    } rings;

    PlanetDesc();
};

// A planetary body which can have an atmosphere or ring system
class Planet
{
public:
    Planet(PlanetDesc& desc);
    virtual ~Planet();

    Vec3 getPosition() const
    {
        return mSceneNode->getPosition();
    }
    Quat getOrientation() const
    {
        return mSceneNode->getOrientation();
    }

    // Accessors
    const PlanetDesc& getDesc() const;
    Ogre::SceneNode* getSceneNode();

    // Inherited from SystemBody
    virtual void update(float dt, Vec3 cameraPosition);

private:
    PlanetDesc mDesc;

    Ogre::SceneNode* mSceneNode;

    // Surface
    Quat mAxialTiltOrientation;
    shared_ptr<Terrain> mTerrain;

    // Atmosphere
    Ogre::MaterialPtr mAtmosphereMaterial;
    Ogre::Entity* mAtmosphereEntity;
    Ogre::MeshPtr mAtmosphereMesh;
    Ogre::SceneNode* mAtmosphereNode;

    // Ring System
    shared_ptr<RingSystem> mRingSystem;
};
