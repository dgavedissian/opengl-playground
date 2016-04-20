/*
  Procedural Planet Prototype
  Copyright (c) 2013-2014 David Avedissian (avedissian.david@gmail.com)
  Author: David Avedissian
*/
#pragma once

class Terrain
{
public:
    Terrain(float radius, Planet* parent);
    ~Terrain();

    void update(const Vec3& cameraPosition);

private:
    typedef unsigned short Index;

    class Vertex
    {
    public:
        Vertex();
        Vertex mid(const Vertex& other);
        void set(const Vertex& other);

        Vec3 v;
        Vec3 n;
        Vec2 uv;
    };

    class PatchCoord
    {
    public:
        PatchCoord();
        PatchCoord(const Vec3& _p, const Vec2& _uv);
        PatchCoord mid(const PatchCoord& other);

        Vec3 p;
        Vec2 uv;
    };

    class Patch
    {
    public:
        Patch(const PatchCoord& p0, const PatchCoord& p1, const PatchCoord& p2,
              const PatchCoord& p3, uint level, Patch* parent, Terrain* context, uint side);
        ~Patch();

        void update(const Vec3& cameraPosition);
        void _addIndices(Ogre::ManualObject* mo);

        void _split(bool splitEdges = true);
        void _join();
        int _findEdgeID(Patch* patch);

        float _getHeight(const Vec2& uv);
        Vec3 _mapToSphere(const Vec3& point);
        Vec3 _calculateNormal(const Vec3& vertex, Vec3** adj);

        PatchCoord p[4];    // patch coords
        Index* v;           // vertices
        Vec3 mCentrePoint;

        bool mHasChildren;
        Patch* child[4];
        Patch* e[4];
        Patch* parent;

        uint mLevel;
        Terrain* mContext;
        uint mSide;
    };

    void _buildMesh(uint side);

    Patch* _allocatePatch(const PatchCoord& p0, const PatchCoord& p1, const PatchCoord& p2,
                          const PatchCoord& p3, uint level, Patch* parent, uint side);
    void _deallocatePatch(Patch* patch);

    Index _allocateVertex(uint side);
    void _deallocateVertex(Index index, uint side);

    float mRadius;

    std::default_random_engine mRandomGenerator;
    float* mDetailThresholdTable;

    // Root patches
    Patch* mRootPatches[6];
    Ogre::ManualObject* mMesh[6];

    // Vertices
    Vertex* mVertices[6];
    std::stack<Index> mFreeIndices[6];
    bool mDirty[6];
};
