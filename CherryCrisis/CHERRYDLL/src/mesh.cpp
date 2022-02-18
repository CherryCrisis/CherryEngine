#include "mesh.hpp"

#include <string>

#include <assimp/mesh.h>

Mesh::~Mesh()
{

}


Resource* Mesh::Create(const char* modelFilepath, const aiMesh* assimpMesh)
{
    std::string meshPath = modelFilepath + std::string("/") + std::string(assimpMesh->mName.C_Str());

	Mesh* mesh = new Mesh(meshPath.c_str());

    for (unsigned int v = 0; v < assimpMesh->mNumVertices; v++)
    {
        Vertex vertex;
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

        mesh->m_vertices.push_back(vertex);
    }

    for (unsigned int f = 0; f < assimpMesh->mNumFaces; f++)
    {
        for (int i = 0; i < 3; ++i)
            mesh->m_indices.push_back(assimpMesh->mFaces[f].mIndices[i]);
    }

	return mesh;
}
