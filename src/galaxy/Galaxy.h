/*
 * Galaxy Prototype
 * Copyright (c) David Avedissian 2014-2015
 */
#pragma once

// Parameters
#define GALAXY_OCTREE_MAX_DEPTH 14
#define GALAXY_DEBUG 0

class Galaxy;

class GalaxyOctreeNode
{
public:
    GalaxyOctreeNode(const glm::vec3& min, const glm::vec3& max, uint level, Galaxy* parent);
    ~GalaxyOctreeNode();

    void update(const glm::vec3& cameraPosition);

private:
    void updateChildren(const glm::vec3& cameraPosition);
    void split();
    void join();

    glm::vec3 mMin;
    glm::vec3 mMax;
    glm::vec3 mCentre;

    uint mLevel;
    Galaxy* mGalaxy;

    bool mHasChildren;
    GalaxyOctreeNode* mChildren[8];
};

class Galaxy
{
public:
    Galaxy(float radius, float thickness, uint64 seed);
    ~Galaxy();

    void update(const glm::vec3& cameraPosition);
    void render();

private:
    float mRadius;
    float mThickness;

};
