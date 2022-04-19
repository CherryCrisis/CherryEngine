#include "pch.hpp"

#include "mesh.hpp"

#include <assimp/mesh.h>

void Mesh::Delete()
{
    m_gpuMesh = nullptr;
}

void Mesh::Load(std::shared_ptr<Mesh> mesh, const aiMesh* assimpMesh)
{
    std::vector<Vertex> vertices;
    vertices.reserve(assimpMesh->mNumVertices);

    for (size_t v = 0u; v < assimpMesh->mNumVertices; v++)
    {
        Vertex& vertex = vertices.emplace_back();
        for (int i = 0; i < 3; ++i)
        {
            vertex.position.data[i] = assimpMesh->mVertices[v][i];
            vertex.normal.data[i] = assimpMesh->mNormals[v][i];

            if (assimpMesh->HasTangentsAndBitangents())
            {
                vertex.tangent.data[i] = assimpMesh->mTangents[v][i];
                vertex.bitangent.data[i] = assimpMesh->mBitangents[v][i];
            }
        }

        if (assimpMesh->HasTextureCoords(0)) {
            vertex.uv.data[0] = assimpMesh->mTextureCoords[0][v].x;
            vertex.uv.data[1] = assimpMesh->mTextureCoords[0][v].y;
        }
        else
        {
            vertex.uv = { 0.f };
        }
    }

    std::vector<unsigned int> indices;
    size_t indicesCount = (size_t)(assimpMesh->mNumFaces) * 3u;
    indices.reserve(indicesCount);

    for (size_t f = 0; f < assimpMesh->mNumFaces; f++)
    {
        for (int i = 0; i < 3; ++i)
        {
            indices.push_back(assimpMesh->mFaces[f].mIndices[i]);
        }
    }

    Mesh::Load(mesh, vertices, indices);
}

void Mesh::Load(std::shared_ptr<Mesh> mesh, std::vector<Vertex>& vertices, std::vector<unsigned int>& indices)
{
    mesh->m_vertices = std::move(vertices);
    mesh->m_indices = std::move(indices);
}

void Mesh::CreateCube(std::shared_ptr<Mesh> mesh, float xHalfRes, float yHalfRes, float zHalfRes)
{
    std::vector<Vertex> vertices;
    vertices.reserve(8);

    for (int i = 0; i < 8; i++)
    {
        int xSign = BoolSign(!BoolPattern(i, 2));
        int ySign = BoolSign(!BoolPattern(i, 4));
        int zSign = BoolSign(!BoolPattern(i, 1));

        Vertex vertex;

        vertex.position.x = xSign * xHalfRes;
        vertex.position.y = ySign *-yHalfRes;
        vertex.position.z = zSign * zHalfRes;
        vertices.push_back(vertex);
    }

    std::vector<unsigned int> indices;
    indices.reserve(36);

    /*Above ABC,BCD*/
    indices.push_back(0); indices.push_back(1); indices.push_back(2);
    indices.push_back(3); indices.push_back(2); indices.push_back(1);

    /*Following EFG,FGH*/
    indices.push_back(6); indices.push_back(5); indices.push_back(4);
    indices.push_back(5); indices.push_back(6); indices.push_back(7);

    /*Left ABF,AEF*/
    indices.push_back(5); indices.push_back(1); indices.push_back(0);
    indices.push_back(0); indices.push_back(4); indices.push_back(5);

    /*Right side CDH,CGH*/
    indices.push_back(2); indices.push_back(3); indices.push_back(7);
    indices.push_back(7); indices.push_back(6); indices.push_back(2);

    /*ACG,AEG*/
    indices.push_back(0); indices.push_back(2); indices.push_back(6);
    indices.push_back(6); indices.push_back(4); indices.push_back(0);

    /*Behind BFH,BDH*/
    indices.push_back(1); indices.push_back(5); indices.push_back(7);
    indices.push_back(7); indices.push_back(3); indices.push_back(1);

    Load(mesh, vertices, indices);
}

void Mesh::CreateQuad(std::shared_ptr<Mesh> mesh, float xHalfRes, float yHalfRes)
{
    std::vector<Vertex> vertices;
    vertices.reserve(4);

    for (int i = 0; i < 4; i++)
    {
        int test = BoolPattern(i, 1);

        int xPattern = !BoolPattern(i + 1, 2);
        int yPattern = !BoolPattern(i, 2);

        Vertex vertex;
        vertex.uv.x = xPattern;
        vertex.uv.y = yPattern;

        int xSign = BoolSign(xPattern);
        int ySign = BoolSign(yPattern);

        vertex.position.x = xSign * xHalfRes;
        vertex.position.y = ySign * yHalfRes;
        vertices.push_back(vertex);
    }

    std::vector<unsigned int> indices;
    indices.reserve(6);

    indices.push_back(0); indices.push_back(1); indices.push_back(2);
    indices.push_back(2); indices.push_back(3); indices.push_back(0);

    Load(mesh, vertices, indices);
}


void Mesh::Reload(const aiMesh* assimpMesh)
{
    m_vertices.clear();
    m_indices.clear();

    m_vertices = std::move(vertices);
    m_indices = std::move(indices);
}
