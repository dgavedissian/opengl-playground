/*
  Procedural Planet Prototype
  Copyright (c) 2013-2014 David Avedissian (avedissian.david@gmail.com)
  Author: David Avedissian
*/
#include "Main.h"
#include "Planet.h"
#include "Terrain.h"

#define TERRAIN_OCTREE_MAX_DEPTH 10
#define TERRAIN_MAX_VERTEX_COUNT 65535

// 7, 15, 25, 35, 55
#define TERRAIN_DETAIL 15    // must be odd

#define V(index) mContext->mVertices[mSide][index]

Terrain::Terrain(float radius, Planet* parent) : mRadius(radius)
{
    // Generate detail threshold table
    mDetailThresholdTable = new float[TERRAIN_OCTREE_MAX_DEPTH];
    mDetailThresholdTable[0] = pow(mRadius, 2);
    for (int i = 1; i < TERRAIN_OCTREE_MAX_DEPTH; ++i)
        mDetailThresholdTable[i] = mDetailThresholdTable[i - 1] * 0.25f;

    // Set up vertices
    for (uint i = 0; i < 6; ++i)
    {
        mVertices[i] = new Vertex[TERRAIN_MAX_VERTEX_COUNT];
        for (int j = TERRAIN_MAX_VERTEX_COUNT - 1; j >= 0; --j)
            mFreeIndices[i].push(j);
    }

    // Build initial cube
    float r = 1.0f;
    Vec3 cube[] = {Vec3(-r, +r, +r), Vec3(+r, +r, +r), Vec3(+r, -r, +r), Vec3(-r, -r, +r),
                   Vec3(-r, +r, -r), Vec3(+r, +r, -r), Vec3(+r, -r, -r), Vec3(-r, -r, -r)};

    static const int edgeIDs[6][4] = {
        {4, 1, 5, 3}, {4, 2, 5, 0}, {4, 3, 5, 1}, {4, 0, 5, 2}, {2, 1, 0, 3}, {0, 1, 2, 3}};

// Allocate root patches
#define PATCH(a, b, c, d, side)                                                                  \
    _allocatePatch(PatchCoord(cube[a], Vec2(0.0f, 0.0f)), PatchCoord(cube[b], Vec2(1.0f, 0.0f)), \
                   PatchCoord(cube[c], Vec2(1.0f, 1.0f)), PatchCoord(cube[d], Vec2(0.0f, 1.0f)), \
                   0, nullptr, side);
    mRootPatches[0] = PATCH(0, 1, 2, 3, 0);
    mRootPatches[1] = PATCH(1, 5, 6, 2, 1);
    mRootPatches[2] = PATCH(5, 4, 7, 6, 2);
    mRootPatches[3] = PATCH(4, 0, 3, 7, 3);
    mRootPatches[4] = PATCH(4, 5, 1, 0, 4);
    mRootPatches[5] = PATCH(3, 2, 6, 7, 5);

    // Set up edge pointers
    for (int i = 0; i < 6; ++i)
    {
        for (int j = 0; j < 4; ++j)
            mRootPatches[i]->e[j] = mRootPatches[edgeIDs[i][j]];

        // Create Ogre object
        mMesh[i] = sceneMgr->createManualObject(generateName("Terrain"));
        parent->getSceneNode()->attachObject(mMesh[i]);

        // Set up manual object
        mMesh[i]->setDynamic(true);
        mMesh[i]->estimateVertexCount(TERRAIN_MAX_VERTEX_COUNT);
        mMesh[i]->estimateIndexCount(1024);
        mMesh[i]->begin("Planet");
        _buildMesh(i);
        mMesh[i]->end();

        // Mark as not-dirty
        mDirty[i] = false;
    }
}

Terrain::~Terrain()
{
    for (uint i = 0; i < 6; ++i)
    {
        _deallocatePatch(mRootPatches[i]);
        delete[] mVertices[i];
    }
    delete[] mDetailThresholdTable;
}

void Terrain::update(const Vec3& cameraPosition)
{
    for (uint i = 0; i < 6; ++i)
    {
        mRootPatches[i]->update(cameraPosition);

        // Rebuild the mesh if out of date
        if (mDirty[i])
        {
            // Update mesh
            mMesh[i]->beginUpdate(0);
            _buildMesh(i);
            mMesh[i]->end();

            // Mark as no longer dirty
            mDirty[i] = false;
        }
    }
}

Terrain::Vertex::Vertex()
{
}

Terrain::Vertex Terrain::Vertex::mid(const Vertex& other)
{
    Vertex out;
    out.v = v.midPoint(other.v);
    out.n = n.midPoint(other.n);
    out.uv = uv.midPoint(other.uv);
    return out;
}

void Terrain::Vertex::set(const Vertex& other)
{
    v = other.v;
    n = other.n;
}

Terrain::PatchCoord::PatchCoord()
{
}

Terrain::PatchCoord::PatchCoord(const Vec3& _p, const Vec2& _uv) : p(_p), uv(_uv)
{
}

Terrain::PatchCoord Terrain::PatchCoord::mid(const PatchCoord& other)
{
    return PatchCoord(p.midPoint(other.p), uv.midPoint(other.uv));
}

Terrain::Patch::Patch(const PatchCoord& p0, const PatchCoord& p1, const PatchCoord& p2,
                      const PatchCoord& p3, uint level, Patch* _parent, Terrain* context, uint side)
    : mLevel(level), parent(_parent), mContext(context), mSide(side), mHasChildren(false)
{
    p[0] = p0;
    p[1] = p1;
    p[2] = p2;
    p[3] = p3;

    // Set up index array
    int detail = TERRAIN_DETAIL;
    v = new Index[detail * detail];

    // Calculate vertices
    for (int y = 0; y < detail; y++)
    {
        for (int x = 0; x < detail; x++)
        {
            // Bilinear interpolation to calculate p and uv
            Vec3 px1 = Ogre::Math::lerp(p0.p, p1.p, (float)x / (detail - 1));
            Vec3 px2 = Ogre::Math::lerp(p3.p, p2.p, (float)x / (detail - 1));
            Vec3 p = Ogre::Math::lerp(px1, px2, (float)y / (detail - 1));

            Vec2 uvx1 = Ogre::Math::lerp(p0.uv, p1.uv, (float)x / (detail - 1));
            Vec2 uvx2 = Ogre::Math::lerp(p3.uv, p2.uv, (float)x / (detail - 1));
            Vec2 uv = Ogre::Math::lerp(uvx1, uvx2, (float)y / (detail - 1));

            // Set vertex
            int i = y * detail + x;
            v[i] = context->_allocateVertex(side);
            V(v[i]).v = _mapToSphere(p) * _getHeight(uv);
            V(v[i]).uv = uv;
        }
    }

    // Calculate normals
    for (int y = 0; y < detail; y++)
    {
        for (int x = 0; x < detail; x++)
        {
            Vec3* adj[] = {
                y > 0 ? &V(v[(y - 1) * detail + x]).v : nullptr,
                x < (detail - 1) ? &V(v[y * detail + (x + 1)]).v : nullptr,
                y < (detail - 1) ? &V(v[(y + 1) * detail + x]).v : nullptr,
                x > 0 ? &V(v[y * detail + (x - 1)]).v : nullptr,
            };
            int i = y * detail + x;
            V(v[i]).n = _calculateNormal(V(v[i]).v, adj);
        }
    }

    // Set centre point
    mCentrePoint = V(v[detail / 2 * detail + detail / 2]).v;

    // Set up child and edge pointers
    for (uint i = 0; i < 4; ++i)
    {
        child[i] = nullptr;
        e[i] = nullptr;
    }
}

Terrain::Patch::~Patch()
{
    if (mHasChildren)
    {
        for (int i = 0; i < 4; ++i)
            mContext->_deallocatePatch(child[i]);
    }
}

void Terrain::Patch::update(const Vec3& cameraPosition)
{
    float distanceSq = mCentrePoint.squaredDistance(cameraPosition);

    if (mHasChildren)
    {
        // If we've moved too far back, merge children
        if (distanceSq > mContext->mDetailThresholdTable[mLevel])
        {
            //_join();
        }
        else
        {
            for (int i = 0; i < 4; ++i)
                child[i]->update(cameraPosition);
        }
    }
    else
    {
        // If we've moved too close, split into 4 children
        if (distanceSq < mContext->mDetailThresholdTable[mLevel] &&
            mLevel < TERRAIN_OCTREE_MAX_DEPTH)
            _split();
    }
}

void Terrain::Patch::_addIndices(Ogre::ManualObject* mo)
{
    if (mHasChildren)
    {
        for (int i = 0; i < 4; ++i)
            child[i]->_addIndices(mo);
    }
    else
    {
        int detail = TERRAIN_DETAIL;
        for (int y = 0; y < (detail - 1); ++y)
        {
            for (int x = 0; x < (detail - 1); ++x)
            {
                mo->index(v[y * detail + x]);
                mo->index(v[(y + 1) * detail + x]);
                mo->index(v[y * detail + (x + 1)]);
                mo->index(v[(y + 1) * detail + x]);
                mo->index(v[(y + 1) * detail + (x + 1)]);
                mo->index(v[y * detail + (x + 1)]);
            }
        }
    }
}

void Terrain::Patch::_split(bool splitEdges)
{
    if (mHasChildren)
        return;

    // Split edges first to maintain only 1 detail level difference
    if (parent)
    {
        // Find child index of this
        int e1;
        for (int i = 0; i < 4; ++i)
        {
            if (parent->child[i] == this)
            {
                e1 = i;
                break;
            }
        }
        int e0 = e1 - 1;
        if (e1 < 0)
            e1 = 3;
        parent->e[e0]->_split();
        parent->e[e1]->_split();
    }

    // Calculate midpoints
    PatchCoord m[5];
    for (int i = 0; i < 4; ++i)
    {
        int j = i + 1;
        if (j > 3)
            j = 0;
        m[i] = p[i].mid(p[j]);
        //    if (e[i]->mHasChildren && mSide == e[i]->mSide)
        //    {
        //      int c = e[i]->_findEdgeID(this);
        //      int cv = c + 1; if (cv > 3) cv = 0;
        //      m[i] = e[i]->child[c]->v[cv];
        //    }
        //    else
        //    {
        //      m[i] = mContext->_allocateVertex(mSide);
        //      V(m[i]) = V(v[i]).mid(V(v[j]));
        //    }
    }
    m[4] = p[0].mid(p[2]);

    // Calculate midpoint normal
    //  Vec3* adj[] = { &V(m[0]).v, &V(m[1]).v, &V(m[2]).v, &V(m[3]).v };
    //  V(m[4]).n = _calculateNormal(V(m[4]).v, adj);

    // Raise midpoints only if we border other patches to prevent gaps
    //  for (int i = 0; i < 4; ++i)
    //  {
    //    if (e[i])
    //    {
    //      if (e[i]->mHasChildren || mSide != e[i]->mSide)
    //      {
    //        // Calculate normal
    //        int j = i + 1; if (j > 3) j = 0;
    //        Vec3* adj[] = { &V(e[i]->mCentre).v, &V(v[j]).v, &V(mCentre).v,
    //        &V(v[i]).v };
    //        V(m[i]).n = _calculateNormal(V(m[i]).v, adj);
    //
    //        // Raise point
    //        V(m[i]).v = _mapToSphere(V(m[i]).p) * _getHeight(V(m[i]).uv);
    //      }
    //    }
    //  }

    // Allocate child patches
    child[0] = mContext->_allocatePatch(p[0], m[0], m[4], m[3], mLevel + 1, this, mSide);
    child[1] = mContext->_allocatePatch(m[0], p[1], m[1], m[4], mLevel + 1, this, mSide);
    child[2] = mContext->_allocatePatch(m[4], m[1], p[2], m[2], mLevel + 1, this, mSide);
    child[3] = mContext->_allocatePatch(m[3], m[4], m[2], p[3], mLevel + 1, this, mSide);

    // Set up edge pointers
    for (int i = 0; i < 4; ++i)
    {
        int j = i + 1;
        if (j > 3)
            j = 0;

        // Neighbours between children
        int k = j + 1;
        if (k > 3)
            k = 0;
        int prev = i - 1;
        if (prev < 0)
            prev = 3;
        child[i]->e[j] = child[j];
        child[i]->e[k] = child[prev];

        // Our neighbours children
        if (e[i])
        {
            if (e[i]->mHasChildren)
            {
                int edge = e[i]->_findEdgeID(this);
                assert(edge != -1);

                int edge2 = edge + 1;
                if (edge2 > 3)
                    edge2 = 0;
                child[i]->e[i] = e[i]->child[edge2];
                child[j]->e[i] = e[i]->child[edge];
                e[i]->child[edge]->e[edge] = child[j];
                e[i]->child[edge2]->e[edge] = child[i];
            }
        }
    }

    // Mark this side as dirty
    mContext->mDirty[mSide] = true;
    mHasChildren = true;
}

void Terrain::Patch::_join()
{
    // Return immediately if no children exist
    if (!mHasChildren)
        return;

    // Return immediately if there are any grand-kids
    for (int i = 0; i < 4; ++i)
    {
        if (child[i]->mHasChildren)
            return;
    }

    // Remove edge references
    for (int i = 0; i < 4; ++i)
    {
        if (e[i])
            if (e[i]->mHasChildren)
            {
                int edge = e[i]->_findEdgeID(this);
                if (edge != -1)
                {
                    int j = edge + 1;
                    if (j > 3)
                        j = 0;
                    e[i]->child[edge]->e[edge] = nullptr;
                    e[i]->child[j]->e[edge] = nullptr;

                    // Correct displaced point
                    // mContext->mVertices[e[i]->child[edge]->v[j]].set(mContext->mVertices[e[i]->child[edge]->v[edge]].mid(mContext->mVertices[e[i]->child[j]->v[j]]));
                }
            }
    }

    // Delete children
    for (int i = 0; i < 4; ++i)
    {
        mContext->_deallocatePatch(child[i]);
        child[i] = nullptr;
    }

    // Mark this side as dirty
    mContext->mDirty[mSide] = true;
    mHasChildren = false;
}

int Terrain::Patch::_findEdgeID(Patch* patch)
{
    for (int i = 0; i < 4; ++i)
    {
        if (patch == e[i])
            return i;
    }
    return -1;
}

float Terrain::Patch::_getHeight(const Vec2& uv)
{
    float scale = 128.0f;
    float scaleSmall = scale * 16.0f;
    float height = 6000.0f;
    float r = mContext->mRadius;
    return r +
           height *
               (scaled_octave_noise_2d(12.0f, 0.5f, 1.0f, 0.0f, 1.0f, uv.x * scale, uv.y * scale) +
                scaled_octave_noise_2d(12.0f, 0.5f, 1.0f, 0.0f, 1.0f, uv.x * scaleSmall,
                                       uv.y * scaleSmall) *
                    0.01f);
}

Vec3 Terrain::Patch::_mapToSphere(const Vec3& point)
{
    Vec3 out;

    // Map the point to a sphere
    float xx = point.x * point.x;
    float yy = point.y * point.y;
    float zz = point.z * point.z;
    out.x = point.x * sqrt(1.0f - yy * 0.5f - zz * 0.5f + yy * zz / 3.0f);
    out.y = point.y * sqrt(1.0f - zz * 0.5f - xx * 0.5f + zz * xx / 3.0f);
    out.z = point.z * sqrt(1.0f - xx * 0.5f - yy * 0.5f + xx * yy / 3.0f);

    return out;
}

Vec3 Terrain::Patch::_calculateNormal(const Vec3& vertex, Vec3** adj)
{
    Vec3 out(0.0f, 0.0f, 0.0f);
    for (int i = 0; i < 4; ++i)
    {
        int j = (i + 1) % 4;
        if (adj[i] && adj[j])
            out += (vertex - *adj[i]).crossProduct(*adj[j] - vertex);
    }
    out.normalise();
    return out;
}

void Terrain::_buildMesh(uint side)
{
    for (uint i = 0; i < TERRAIN_MAX_VERTEX_COUNT; ++i)
    {
        mMesh[side]->position(mVertices[side][i].v);
        mMesh[side]->normal(mVertices[side][i].n);
        mMesh[side]->textureCoord(mVertices[side][i].uv);
    }
    mRootPatches[side]->_addIndices(mMesh[side]);
}

Terrain::Patch* Terrain::_allocatePatch(const PatchCoord& p0, const PatchCoord& p1,
                                        const PatchCoord& p2, const PatchCoord& p3, uint level,
                                        Patch* parent, uint side)
{
    return new Patch(p0, p1, p2, p3, level, parent, this, side);
}

void Terrain::_deallocatePatch(Patch* patch)
{
    delete patch;
}

Terrain::Index Terrain::_allocateVertex(uint side)
{
    Index index = mFreeIndices[side].top();
    mFreeIndices[side].pop();
    return index;
}

void Terrain::_deallocateVertex(Index index, uint side)
{
    mFreeIndices[side].push(index);
}
