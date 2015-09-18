/*
 * Galaxy Prototype
 * Copyright (c) David Avedissian 2014-2015
 */
#include "framework/Common.h"
#include "Galaxy.h"

GalaxyOctreeNode::GalaxyOctreeNode(const glm::vec3& min, const glm::vec3& max, uint level, Galaxy* parent)
    : mMin(min),
      mMax(max),
      mCentre((min + max) * 0.5f),
      mLevel(level),
      mGalaxy(parent),
      mHasChildren(false)
{
}

GalaxyOctreeNode::~GalaxyOctreeNode()
{
    if (mHasChildren)
        Join();
}

void GalaxyOctreeNode::Update(const glm::vec3& cameraPosition)
{
}

void GalaxyOctreeNode::UpdateChildren(const glm::vec3& cameraPosition)
{

}

void GalaxyOctreeNode::Split()
{

}

void GalaxyOctreeNode::Join()
{

}

Galaxy::Galaxy(float radius, float thickness, uint64 seed)
    : mRadius(radius),
      mThickness(thickness)
{
}

Galaxy::~Galaxy()
{
}

void Galaxy::Update(const glm::vec3& cameraPosition)
{
}

void Galaxy::Render()
{
}
