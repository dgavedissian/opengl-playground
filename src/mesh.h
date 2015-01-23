/*
 * GL Framework
 * Copyright (c) David Avedissian 2014-2015
 */
#ifndef MESH_H
#define MESH_H

class Mesh
{
public:
    class Builder
    {
    public:
        friend class Mesh;

        Builder();
        ~Builder();

        Builder& vertices(float* data, uint size);
        Builder& elements(uint* data, uint size);
        shared_ptr<Mesh> build();

    private:
        float* mVertexData;
        uint mVertexDataSize;
        uint* mElementData;
        uint mElementDataSize;
    };

    Mesh(Builder& builder);
    ~Mesh();

    void bind();
    void draw();

private:
    GLuint mVao, mVbo, mEbo;
    uint mVertexCount;
};

#endif /* MESH_H */
