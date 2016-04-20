/*
  Procedural Planet Prototype
  Copyright (c) 2013-2014 David Avedissian (avedissian.david@gmail.com)
  Author: David Avedissian
*/
#include "Main.h"
#include "RingSystem.h"
#include "Planet.h"

RingSystem::RingSystem(PlanetDesc& desc, Planet* parent)
    : mMinRadius(desc.rings.minRadius), mMaxRadius(desc.rings.maxRadius), mParent(parent)
{
    // Generate detail threshold table
    mDetailThresholdTable[0] = pow(mMaxRadius * 2.0f, 2);
    for (int i = 1; i < RING_OCTREE_MAX_DEPTH; i++)
        mDetailThresholdTable[i] = mDetailThresholdTable[i - 1] * 0.25f;

    // Generate the list of vertices used by the ring mesh
    uint detail = 16;
    vector<Vec2> vertices;
    vector<Vec2> texCoords;
    float cosDetail = cos(Ogre::Math::PI / (float)detail);
    float maxRadius = desc.rings.maxRadius / cosDetail;
    float minRadius = desc.rings.minRadius * cosDetail;
    for (uint i = 0; i < detail; ++i)
    {
        float angle = ((float)i / (float)detail) * Ogre::Math::PI * 2.0f;
        Vec2 pos(sin(angle), cos(angle));
        vertices.push_back(pos * maxRadius);
        vertices.push_back(pos * minRadius);
    }

    // Create the mesh from the data
    string ringMeshName = generateName("RingMesh");
    Ogre::ManualObject* ringMesh = sceneMgr->createManualObject();
    ringMesh->estimateVertexCount(vertices.size());
    ringMesh->begin("");

    // Add the vertices
    for (uint i = 0; i < vertices.size(); ++i)
    {
        Vec3 transformedPoint(vertices[i].x, 0.0f, vertices[i].y);
        ringMesh->position(transformedPoint);
    }

    // Add the indices
    for (uint i = 0; i < detail; ++i)
    {
        int startIndex = i * 2;
        if (i < (detail - 1))
        {
            ringMesh->index(startIndex);
            ringMesh->index(startIndex + 2);
            ringMesh->index(startIndex + 1);
            ringMesh->index(startIndex + 2);
            ringMesh->index(startIndex + 3);
            ringMesh->index(startIndex + 1);
        }
        else
        {
            ringMesh->index(startIndex);
            ringMesh->index(0);
            ringMesh->index(startIndex + 1);
            ringMesh->index(0);
            ringMesh->index(1);
            ringMesh->index(startIndex + 1);
        }
    }

    // Finish adding vertices
    ringMesh->end();

    // Convert this manual object into a mesh and free the buffers
    mRingMesh = ringMesh->convertToMesh(ringMeshName);
    sceneMgr->destroyManualObject(ringMesh);

    // Create the entity and attach
    mRingMaterial = cloneMaterial("Universe/Planet/Rings", generateName("ringsMaterial"));
    mRingMaterial->getTechnique(0)->getPass(0)->getTextureUnitState("rings")->setTextureName(
        desc.rings.texture);
    mRingEntity = sceneMgr->createEntity(mRingMesh);
    mRingEntity->setMaterial(mRingMaterial);
    mRingEntity->setQueryFlags(1 << 1);
    mSceneNode = parent->getSceneNode()->createChildSceneNode();
    mSceneNode->attachObject(mRingEntity);

    // Load the texture
    mRingTexture.load(desc.rings.texture, "General");

    // Update shader parameters
    float minDistance = sqrt(mDetailThresholdTable[RING_OCTREE_MAX_DEPTH - 6]);
    mRingEntity->getSubEntity(0)
        ->setCustomParameter(0, Ogre::Vector4(desc.radius, 0.0f, 0.0f, 0.0f));
    mRingEntity->getSubEntity(0)
        ->setCustomParameter(1, Ogre::Vector4(desc.rings.minRadius, desc.rings.maxRadius,
                                              desc.rings.maxRadius - desc.rings.minRadius, 0.0f));
    mRingEntity->getSubEntity(0)->setCustomParameter(
        3, Ogre::Vector4(1.0f / (desc.atmosphere.radius - desc.radius), 0.0f, 0.0f, 0.0f));
    mRingEntity->getSubEntity(0)->setCustomParameter(4, Ogre::Vector4(1.0f, 0.0f, 0.0f, 0.0f));
    mRingEntity->getSubEntity(0)
        ->setCustomParameter(7, Ogre::Vector4(minDistance * 0.03125f, minDistance, 0.0f, 0.0f));

    // Set up the random number generator - doesnt matter what this value is as
    // long as it never changes
    mRandomGenerator.seed(0xDEADBEEF);

    // Set up the billboard set
    mParticlesLarge = sceneMgr->createBillboardSet(100000);
    mParticlesLarge->setMaterialName("Universe/Planet/Rings/Dust");
    mParticlesLarge->setQueryFlags(1 << 1);
    mSceneNode->attachObject(mParticlesLarge);
    mParticlesSmall = sceneMgr->createBillboardSet(100000);
    mParticlesSmall->setMaterialName("Universe/Planet/Rings/Dust");
    mParticlesLarge->setQueryFlags(1 << 1);
    mSceneNode->attachObject(mParticlesSmall);

    // Create the root ring node
    // x--------x
    // |        |
    // |        |  width & height = maxRadius * 2
    // |        |
    // x--------x
    mDetailRootNode =
        new Patch(Vec2(-mMaxRadius, mMaxRadius), Vec2(mMaxRadius, -mMaxRadius), 0, this);
}

RingSystem::~RingSystem()
{
    delete mDetailRootNode;
}

void RingSystem::update(const Vec3& cameraPosition)
{
    mDetailRootNode->update(cameraPosition);
}

Ogre::ColourValue RingSystem::getColour(const Vec2& position) const
{
    float distance = position.length();
    float index = (distance - mMinRadius) / (mMaxRadius - mMinRadius);

    // Trim the outer parts of the ring.
    if (index < 0.0f || index > 1.0f)
        return Ogre::ColourValue::Black;

    size_t scaledIndex = (size_t)floor(index * (float)mRingTexture.getWidth());
    Ogre::ColourValue colour = mRingTexture.getColourAt(scaledIndex, 0, 0);
    colour.a = 1.0f;
    return colour;
}

float RingSystem::getDensity(const Vec2& position) const
{
    float distance = position.length();
    float index = (distance - mMinRadius) / (mMaxRadius - mMinRadius);

    // Trim the outer parts of the ring.
    if (index < 0.0f || index > 1.0f)
        return 0.0f;

    size_t scaledIndex = (size_t)floor(index * (float)mRingTexture.getWidth());
    return mRingTexture.getColourAt(scaledIndex, 0, 0).a;
}

Planet* RingSystem::getParent()
{
    return mParent;
}

float RingSystem::getThickness() const
{
    return 1500.0f;
}

Ogre::Entity* RingSystem::getEntity()
{
    return mRingEntity;
}

std::default_random_engine& RingSystem::getGenerator()
{
    return mRandomGenerator;
}

Ogre::BillboardSet* RingSystem::getLargeParticleSet()
{
    return mParticlesLarge;
}

Ogre::BillboardSet* RingSystem::getSmallParticleSet()
{
    return mParticlesSmall;
}

RingSystem::Patch::Patch(const Vec2& min, const Vec2& max, uint level, RingSystem* ringSystem)
    : mMin(min),
      mMax(max),
      mCentre((min + max) * 0.5f),
      mLevel(level),
      mParentRingSystem(ringSystem),
      mHasChildren(false)
{
    _cache();
}

RingSystem::Patch::~Patch()
{
    _free();
}

void RingSystem::Patch::update(const Vec3& cameraPosition)
{
    float distance = cameraPosition.squaredDistance(Vec3(mCentre.x, 0.0f, mCentre.y));

    // Swap between billboards and meshes if they're within the right distance
    for (auto i = mAsteroidList.begin(); i != mAsteroidList.end(); i++)
    {
        float sqDistance = cameraPosition.squaredDistance((*i).node->getPosition());

        if (sqDistance < mParentRingSystem->mDetailThresholdTable[RING_OCTREE_MAX_DEPTH - 1])
        {
            if (!(*i).node->isInSceneGraph())
            {
                // mParentRingSystem->getParent()->getSurfaceNode()->addChild((*i).node);
                mParentRingSystem->getLargeParticleSet()->removeBillboard((*i).billboard);

                /*
                // Update the physical body
                PhysicsSystem::inst().addToWorld((*i).rigidBody);
                btTransform& xform = (*i).rigidBody->getWorldTransform();
                xform.setOrigin(Vec3((*i).node->_getDerivedPosition()));
                xform.setRotation(Quat((*i).node->_getDerivedOrientation()));
                */
            }
        }
        else
        {
            if ((*i).node->isInSceneGraph())
            {
                // PhysicsSystem::inst().removeFromWorld((*i).rigidBody);
                // mParentRingSystem->getParent()->getSurfaceNode()->removeChild((*i).node);
                (*i).billboard = mParentRingSystem->getLargeParticleSet()->createBillboard(
                    (*i).node->getPosition(), (*i).colour);
                (*i).billboard->setDimensions((*i).size, (*i).size);
            }
        }
    }

    if (mHasChildren)
    {
        // Combine the node if it has children if the distance is too far
        if (distance > mParentRingSystem->mDetailThresholdTable[mLevel])
        {
            _join();
        }
        else
        {
            // Otherwise update...
            for (int i = 0; i < 4; i++)
                mChildren[i]->update(cameraPosition);
        }
    }
    else
    {
        // Split the node into 4 children if the distance threshold is reached
        if (distance < mParentRingSystem->mDetailThresholdTable[mLevel] &&
            mLevel < (RING_OCTREE_MAX_DEPTH - 1))
        {
            _split();

            // Update those children incase the camera has moved VERY quickly
            assert(mHasChildren == true);

            for (int i = 0; i < 4; i++)
                mChildren[i]->update(cameraPosition);
        }
    }
}

void RingSystem::Patch::_cache()
{
    std::uniform_real_distribution<float> uniformDist(0.0f, 1.0f);
    std::uniform_int_distribution<int> asteroidTypeDistribution(1, 3);
    std::uniform_real_distribution<float> asteroidRotationDistribution(0.0f, Ogre::Math::PI);
    std::normal_distribution<float> asteroidDistribution(0.0f, 0.1f);
    std::normal_distribution<float> dustDistribution(0.0f, 0.1f);

    // Generate asteroids
    if (mLevel > 6)
    {
        int noAsteroids = (int)pow(1.7f, (int)(mLevel - 6)) * 2;
        float asteroidSize = mParentRingSystem->getThickness() * 0.4f / (float)(mLevel - 6);

        for (int i = 0; i < noAsteroids; i++)
        {
            // Generate position
            Vec3 position(
                Ogre::Math::lerp(mMin.x, mMax.x, uniformDist(mParentRingSystem->getGenerator())),
                mParentRingSystem->getThickness() *
                    asteroidDistribution(mParentRingSystem->getGenerator()),
                Ogre::Math::lerp(mMin.y, mMax.y, uniformDist(mParentRingSystem->getGenerator())));

            // Generate a random number in the range [0;1] and if that number is less
            // than the density probability then spawn an asteroid there
            if (uniformDist(mParentRingSystem->getGenerator()) <
                mParentRingSystem->getDensity(Ogre::Vector2(position.x, position.z)))
            {
                Asteroid asteroid;
                asteroid.size = asteroidSize;

                // Create a billboard
                asteroid.colour =
                    mParentRingSystem->getColour(Ogre::Vector2(position.x, position.z));
                asteroid.billboard = mParentRingSystem->getLargeParticleSet()->createBillboard(
                    position, asteroid.colour);
                asteroid.billboard->setDimensions(asteroidSize, asteroidSize);

                // Create the entity
                asteroid.node = nullptr;
                Ogre::Entity* entity = sceneMgr->createEntity(
                    "asteroid" +
                    std::to_string(asteroidTypeDistribution(mParentRingSystem->getGenerator())) +
                    ".mesh");
                entity->setMaterialName("system/planet/rings/asteroid");
                asteroid.node = sceneMgr->createSceneNode();
                asteroid.node->attachObject(entity);
                asteroid.node->setPosition(position);
                asteroid.node->setOrientation(Quat(Ogre::Radian(asteroidRotationDistribution(
                                                       mParentRingSystem->getGenerator())),
                                                   Vec3::UNIT_X) *
                                              Quat(Ogre::Radian(asteroidRotationDistribution(
                                                       mParentRingSystem->getGenerator())),
                                                   Vec3::UNIT_Y));
                asteroid.node->setScale(Vec3(asteroidSize));

                /*
                // Create the rigid body
                Ogre::Matrix4 scaleTransform = Ogre::Matrix4::IDENTITY;
                scaleTransform.setScale(Vec3(asteroidSize));
                BtOgre::StaticMeshToShapeConverter entityCollisionMesh(entity,
                scaleTransform);
                asteroid.rigidBody = PhysicsSystem::inst().createRigidBody(0.0f,
                entityCollisionMesh.createConvex());
                */

                // Spawn an asteroid
                mAsteroidList.push_back(asteroid);
            }
        }
    }

    // Generate dust particles
    if (mLevel > 5)
    {
        int noDustParticles = (int)pow(2.0f, (int)(mLevel - 5)) * 3;
        float dustParticleSize = mParentRingSystem->getThickness() * 0.05f / (float)(mLevel - 5);

        for (int i = 0; i < noDustParticles; i++)
        {
            // Generate position
            Vec3 position(
                Ogre::Math::lerp(mMin.x, mMax.x, uniformDist(mParentRingSystem->getGenerator())),
                mParentRingSystem->getThickness() * 0.5f *
                    dustDistribution(mParentRingSystem->getGenerator()),
                Ogre::Math::lerp(mMin.y, mMax.y, uniformDist(mParentRingSystem->getGenerator())));

            // Generate a random number in the range [0;1] and if that number is less
            // than the density probability then spawn an asteroid there
            if (uniformDist(mParentRingSystem->getGenerator()) <
                mParentRingSystem->getDensity(Ogre::Vector2(position.x, position.z)))
            {
                Ogre::ColourValue colour =
                    mParentRingSystem->getColour(Ogre::Vector2(position.x, position.z));
                Ogre::Billboard* billboard =
                    mParentRingSystem->getSmallParticleSet()->createBillboard(position, colour);
                billboard->setDimensions(dustParticleSize, dustParticleSize);
                mDustList.push_back(billboard);
            }
        }
    }
}

void RingSystem::Patch::_free()
{
    // Destroy billboards and meshes
    for (auto i = mAsteroidList.begin(); i != mAsteroidList.end(); i++)
    {
        // Only remove the billboard if it's being used instead of the mesh
        if (!(*i).node->isInSceneGraph())
            mParentRingSystem->getLargeParticleSet()->removeBillboard((*i).billboard);
        // else
        //  PhysicsSystem::inst().removeFromWorld((*i).rigidBody);

        sceneMgr->destroyEntity(static_cast<Ogre::Entity*>((*i).node->getAttachedObject(0)));
        sceneMgr->destroySceneNode((*i).node);
        // PhysicsSystem::inst().destroyCollisionShape((*i).rigidBody->getCollisionShape());
        // PhysicsSystem::inst().destroyRigidBody((*i).rigidBody);
    }

    for (auto i = mDustList.begin(); i != mDustList.end(); i++)
        mParentRingSystem->getSmallParticleSet()->removeBillboard((*i));

    mAsteroidList.clear();
    mDustList.clear();

    // Destroy children
    if (mHasChildren)
        _join();
}

void RingSystem::Patch::_split()
{
    // TODO - pool PlanetRingSystemDetailNode's to save all these allocations!
    mChildren[0] = new Patch(mMin, mCentre, mLevel + 1, mParentRingSystem);
    mChildren[1] = new Patch(Ogre::Vector2(mCentre.x, mMin.y), Ogre::Vector2(mMax.x, mCentre.y),
                             mLevel + 1, mParentRingSystem);
    mChildren[2] = new Patch(Ogre::Vector2(mMin.x, mCentre.y), Ogre::Vector2(mCentre.x, mMax.y),
                             mLevel + 1, mParentRingSystem);
    mChildren[3] = new Patch(mCentre, mMax, mLevel + 1, mParentRingSystem);
    mHasChildren = true;
}

void RingSystem::Patch::_join()
{
    for (int i = 0; i < 4; i++)
        delete mChildren[i];
    mHasChildren = false;
}
