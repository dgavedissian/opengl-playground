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

    void Update(const glm::vec3& cameraPosition);

private:
    void UpdateChildren(const glm::vec3& cameraPosition);
    void Split();
    void Join();

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

    void Update(const glm::vec3& cameraPosition);
    void Render();

private:
    float mRadius;
    float mThickness;

};
