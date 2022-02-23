#include "mesh.hpp"

#include <string>

#include <assimp/mesh.h>

Resource* Mesh::Create(const char* modelFilepath, const aiMesh* assimpMesh)
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

    // TODO: Change with string view
    std::string meshPath = modelFilepath + std::string("/") + std::string(assimpMesh->mName.C_Str());

	return Mesh::Create(meshPath.c_str(), vertices, indices);
}

Resource* Mesh::Create(const char* modelName, std::vector<Vertex>& vertices, std::vector<unsigned int>& indices)
{
    Mesh* mesh = new Mesh(modelName);

    mesh->m_vertices = std::move(vertices);
    mesh->m_indices = std::move(indices);

    return mesh;
}

Resource* Mesh::CreateCube(const char* cubeName, float xHalfRes, float yHalfRes, float zHalfRes)
{
    std::vector<Vertex> vertices;
    vertices.reserve(8);

    vertices.push_back(Vertex({-xHalfRes, yHalfRes,-zHalfRes}));//Point A 0
    vertices.push_back(Vertex({-xHalfRes, yHalfRes, zHalfRes}));//Point B 1
    vertices.push_back(Vertex({ xHalfRes, yHalfRes,-zHalfRes}));//Point C 2
    vertices.push_back(Vertex({ xHalfRes, yHalfRes, zHalfRes}));//Point D 3

    vertices.push_back(Vertex({-xHalfRes,-yHalfRes,-zHalfRes}));//Point E 4
    vertices.push_back(Vertex({-xHalfRes,-yHalfRes, zHalfRes}));//Point F 5
    vertices.push_back(Vertex({ xHalfRes,-yHalfRes,-zHalfRes}));//Point G 6
    vertices.push_back(Vertex({ xHalfRes,-yHalfRes, zHalfRes}));//Point H 7

    std::vector<unsigned int> indices;
    indices.reserve(36);

    /*Above ABC,BCD*/
    indices.push_back(0); indices.push_back(1); indices.push_back(2);
    indices.push_back(1); indices.push_back(2); indices.push_back(3);

    /*Following EFG,FGH*/
    indices.push_back(4); indices.push_back(5); indices.push_back(6);
    indices.push_back(5); indices.push_back(6); indices.push_back(7);

    /*Left ABF,AEF*/
    indices.push_back(0); indices.push_back(1); indices.push_back(5);
    indices.push_back(0); indices.push_back(4); indices.push_back(5);

    /*Right side CDH,CGH*/
    indices.push_back(2); indices.push_back(3); indices.push_back(7);
    indices.push_back(2); indices.push_back(6); indices.push_back(7);

    /*ACG,AEG*/
    indices.push_back(0); indices.push_back(2); indices.push_back(6);
    indices.push_back(0); indices.push_back(4); indices.push_back(6);

    /*Behind BFH,BDH*/
    indices.push_back(1); indices.push_back(5); indices.push_back(7);
    indices.push_back(1); indices.push_back(3); indices.push_back(7);

    return Mesh::Create(cubeName, vertices, indices);
}
