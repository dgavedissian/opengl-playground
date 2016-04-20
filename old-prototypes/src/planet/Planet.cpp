/*
  Procedural Planet Prototype
  Copyright (c) 2013-2014 David Avedissian (avedissian.david@gmail.com)
  Author: David Avedissian
*/
#include "Main.h"
#include "Planet.h"

PlanetDesc::PlanetDesc()
{
    radius = 0.0f;
    rotationPeriod = 0.0f;
    axialTilt = 0.0f;
    surfaceTexture = "";
    nightTexture = "";
    hasAtmosphere = false;
    atmosphere.radius = 0.0f;
    hasRings = false;
    rings.minRadius = 0.0f;
    rings.maxRadius = 0.0f;
    rings.texture = "";
}

Planet::Planet(PlanetDesc& desc)
    : mDesc(desc),
      mAxialTiltOrientation(Ogre::Radian(-mDesc.axialTilt), Vec3::UNIT_Z),
      mTerrain(nullptr),
      mAtmosphereEntity(nullptr),
      mAtmosphereNode(nullptr),
      mRingSystem(nullptr)
{
    assert(desc.radius > 0.0f);

    mSceneNode = sceneMgr->getRootSceneNode()->createChildSceneNode();

    // Create the surface
    mTerrain.reset(new Terrain(desc.radius, this));

    // Create the atmosphere
    if (desc.hasAtmosphere)
    {
        mAtmosphereMesh =
            generateSphereMesh(generateName("Sphere"), desc.atmosphere.radius, 64, 64, true, true);
        mAtmosphereMaterial =
            cloneMaterial("Universe/Planet/Atmosphere", generateName("atmosphereMaterial"));
        mAtmosphereEntity = sceneMgr->createEntity(mAtmosphereMesh);
        mAtmosphereEntity->setMaterial(mAtmosphereMaterial);
        mAtmosphereEntity->setQueryFlags(1 << 1);
        mAtmosphereNode = sceneMgr->getRootSceneNode()->createChildSceneNode();
        mAtmosphereNode->attachObject(mAtmosphereEntity);

        // Compute and set shader constants
        Vec3 wavelength(0.65f, 0.57f, 0.475f);    // 650nm for red, 570mn for green, 475nm for blue
        Vec3 invWavelength(1.0f / pow(wavelength.x, 4.0f), 1.0f / pow(wavelength.y, 4.0f),
                           1.0f / pow(wavelength.z, 4.0f));
        float Kr = 0.0025f, Km = 0.0015f, ESun = 15.0f,
              scale = 1.0f / (desc.atmosphere.radius - desc.radius), scaleDepth = 0.25f, g = -0.95f;

        /*
        mSurfaceEntity->getSubEntity(0)->setCustomParameter(5,
        Ogre::Vector4(invWavelength.x, invWavelength.y, invWavelength.z, 0.0f));
        mSurfaceEntity->getSubEntity(0)->setCustomParameter(6,
        Ogre::Vector4(desc.radius, desc.radius * desc.radius,
        desc.atmosphere.radius, desc.atmosphere.radius * desc.atmosphere.radius));
        mSurfaceEntity->getSubEntity(0)->setCustomParameter(7, Ogre::Vector4(Kr *
        ESun, Kr * 4.0f * Ogre::Math::PI, Km * ESun, Km * 4.0f * Ogre::Math::PI));
        mSurfaceEntity->getSubEntity(0)->setCustomParameter(8, Ogre::Vector4(scale,
        scaleDepth, scale / scaleDepth, 0.0f));
        */

        mAtmosphereEntity->getSubEntity(0)->setCustomParameter(
            0, Ogre::Vector4(invWavelength.x, invWavelength.y, invWavelength.z, 0.0f));
        mAtmosphereEntity->getSubEntity(0)->setCustomParameter(
            1, Ogre::Vector4(desc.radius, desc.radius * desc.radius, desc.atmosphere.radius,
                             desc.atmosphere.radius * desc.atmosphere.radius));
        mAtmosphereEntity->getSubEntity(0)
            ->setCustomParameter(2, Ogre::Vector4(Kr * ESun, Kr * 4.0f * Ogre::Math::PI, Km * ESun,
                                                  Km * 4.0f * Ogre::Math::PI));
        mAtmosphereEntity->getSubEntity(0)
            ->setCustomParameter(3, Ogre::Vector4(scale, scaleDepth, scale / scaleDepth, 0.0f));
        mAtmosphereEntity->getSubEntity(0)
            ->setCustomParameter(7, Ogre::Vector4(g, g * g, 0.0f, 0.0f));
    }

    // Create the ring system
    if (desc.hasRings)
    {
        mRingSystem.reset(new RingSystem(desc, this));

        // Set ring system properties on the surface shader
        Ogre::Vector4 ringDimensions(Vec3(desc.rings.minRadius, desc.rings.maxRadius,
                                          desc.rings.maxRadius - desc.rings.minRadius));
        // mSurfaceMaterial->getTechnique(0)->getPass(0)->getTextureUnitState("rings")->setTextureName(desc.rings.texture);
        // mSurfaceEntity->getSubEntity(0)->setCustomParameter(4, ringDimensions);
    }

    // Configure planet shader
    // mSurfaceEntity->getSubEntity(0)->setCustomParameter(0,
    // Ogre::Vector4(desc.nightTexture.length() > 0 ? 1.0f : 0.0f,
    // desc.hasAtmosphere, desc.hasRings, 0.0f));
}

Planet::~Planet()
{
    mRingSystem.reset();

    if (mAtmosphereEntity)
    {
        sceneMgr->destroySceneNode(mAtmosphereNode);
        sceneMgr->destroyEntity(mAtmosphereEntity);
        mAtmosphereMaterial.setNull();
        mAtmosphereMesh.setNull();
    }

    mTerrain.reset();

    sceneMgr->destroySceneNode(mSceneNode);
}

void Planet::update(float dt, Vec3 cameraPosition)
{
    // PROTOTYPE - MODIFIED
    Vec3 cameraSpacePosition =
        mSceneNode->getPosition() - cameraPosition;    // mPosition.getRelativeToCamera();

    // Updates based on object position - only needs to change if object position
    // changes

    // Decide whether we are inside the planets "region"
    // PROTOTYPE - MODIFIED
    /*
    float regionRadius = mDesc.hasRings ? mDesc.rings.maxRadius : mDesc.radius *
    2.0f;
    regionRadius *= regionRadius; // Multiply by self as we're comparing squared
    distances
    float scale = 1.0f;
    if (cameraSpacePosition.LengthSq() > regionRadius)
    {
      // The planet needs to be scaled so that is resides within the far clip
    distance
      float maxRadius = max(mDesc.radius, max(mDesc.atmosphere.radius,
    mDesc.rings.maxRadius));
      scale = calculateScaleFactor(cameraSpacePosition, maxRadius);
    }

    // Scale the surface and atmosphere
    mSurfaceNode->setPosition(cameraSpacePosition * scale);
    mSurfaceNode->setScale(Ogre::Vector3(scale));
    if (mDesc.hasAtmosphere)
    {
      mAtmosphereNode->setPosition(mSurfaceNode->getPosition());
      mAtmosphereNode->setScale(Ogre::Vector3(scale));
    }
    */
    float scale = 1.0f;
    float scaledRadius = mDesc.radius * scale, scaledAtmoRadius = mDesc.atmosphere.radius * scale;

    // Detect a sun object
    // TODO support multiple stars
    // PROTOTYPE - MODIFIED
    // if (StarSystem::inst().getStars().size() > 0)
    {
        // Position sunPosition =
        // (*StarSystem::inst().getStars().begin())->getPosition();
        Vec3 sunDirection =
            -lightDirection;    // -mPosition.getRelativeToPoint(sunPosition).Normalized();
        Vec3 localSunDirection = mAxialTiltOrientation.Inverse() * sunDirection;

        // Set the direction in the planet shader
        // mSurfaceEntity->getSubEntity(0)->setCustomParameter(1,
        // Ogre::Vector4(mSurfaceNode->getPosition()));
        // mSurfaceEntity->getSubEntity(0)->setCustomParameter(2,
        // Ogre::Vector4(sunDirection));
        // mSurfaceEntity->getSubEntity(0)->setCustomParameter(3,
        // Ogre::Vector4(localSunDirection));

        // Set the direction in the atmosphere shader
        if (mDesc.hasAtmosphere)
            mAtmosphereEntity->getSubEntity(0)->setCustomParameter(
                4, Ogre::Vector4(sunDirection.x, sunDirection.y, sunDirection.z, 0.0f));

        // Update ring shader parameters
        if (mRingSystem)
        {
            // Update ring dimensions
            Ogre::Vector4 ringDimensions(Vec3(mDesc.rings.minRadius, mDesc.rings.maxRadius,
                                              mDesc.rings.maxRadius - mDesc.rings.minRadius) *
                                         scale);
            // mSurfaceEntity->getSubEntity(0)->setCustomParameter(4, ringDimensions);

            // Set shader parameters
            // Vec3 sunToPlanet = getPosition().getRelativeTo(sunPosition);
            Vec3 sunPosition = getPosition() + sunDirection * 1e9f;
            Vec3 sunToPlanet = getPosition() - sunPosition;
            mRingSystem->getEntity()->getSubEntity(0)->setCustomParameter(
                0, Ogre::Vector4(scaledRadius, 0.0f, 0.0f, 0.0f));
            mRingSystem->getEntity()->getSubEntity(0)->setCustomParameter(
                1, Ogre::Vector4(ringDimensions));
            mRingSystem->getEntity()->getSubEntity(0)->setCustomParameter(
                2, Ogre::Vector4(getPosition()));
            mRingSystem->getEntity()->getSubEntity(0)->setCustomParameter(
                3, Ogre::Vector4(1.0f / (scaledAtmoRadius - scaledRadius), 0.0f, 0.0f, 0.0f));
            mRingSystem->getEntity()->getSubEntity(0)->setCustomParameter(
                5,
                Ogre::Vector4(sunToPlanet.x, sunToPlanet.y, sunToPlanet.z, sunToPlanet.length()));
            mRingSystem->getEntity()->getSubEntity(0)->setCustomParameter(
                6, Ogre::Vector4(sunPosition));
        }
    }

    // Updates based on camera position
    ////////////////////
    Vec3 localCameraPosition = -cameraSpacePosition;

    // Update terrain
    mTerrain->update(localCameraPosition);

    // Update rings
    if (mDesc.hasRings)
        mRingSystem->update(getOrientation().Inverse() * localCameraPosition);

    // Update atmosphere shader
    if (mDesc.hasAtmosphere)
    {
        float invScale = 1.0f / scale;
        float camHeight = localCameraPosition.length() * invScale;
        // mSurfaceEntity->getSubEntity(0)->setCustomParameter(10,
        // Ogre::Vector4(localCameraPosition.x, localCameraPosition.y,
        // localCameraPosition.z, 0.0f) * invScale);
        // mSurfaceEntity->getSubEntity(0)->setCustomParameter(11,
        // Ogre::Vector4(camHeight, camHeight * camHeight, 0.0f, 0.0f));
        mAtmosphereEntity->getSubEntity(0)
            ->setCustomParameter(5, Ogre::Vector4(localCameraPosition.x, localCameraPosition.y,
                                                  localCameraPosition.z, 0.0f) *
                                        invScale);
        mAtmosphereEntity->getSubEntity(0)
            ->setCustomParameter(6, Ogre::Vector4(camHeight, camHeight * camHeight, 0.0f, 0.0f));
    }

    // Update satellites
    // SystemBody::update(dt, cameraPosition);
}

const PlanetDesc& Planet::getDesc() const
{
    return mDesc;
}

Ogre::SceneNode* Planet::getSceneNode()
{
    return mSceneNode;
}
