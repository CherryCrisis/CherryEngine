#include "model.hpp"

#include <assimp/mesh.h>
#include <assimp/scene.h>


#include "resourceManager.hpp"

Resource* Model::Create(const char* filepath, const aiScene* assimpScene, const aiNode* assimpNode)
{
	ResourceManager* resourceManager = ResourceManager::GetInstance();

	Model* model = new Model(filepath);

	size_t meshId = (size_t)assimpNode->mMeshes[0];
	const aiMesh* assimpMesh = assimpScene->mMeshes[meshId];

	model->m_mesh = resourceManager->AddResource<Mesh>(filepath, false, assimpMesh);

	return model;
}

Model::~Model()
{
	m_mesh = nullptr;
}
