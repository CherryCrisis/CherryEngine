#include "model.hpp"

#include <assimp/mesh.h>
#include <assimp/material.h>
#include <assimp/scene.h>

#include "resourceManager.hpp"

#include "material.hpp"
#include "mesh.hpp"

Resource* Model::Create(const char* filepath, const aiScene* assimpScene, const aiNode* assimpNode)
{
	ResourceManager* resourceManager = ResourceManager::GetInstance();

	Model* model = new Model(filepath);

	size_t meshId = (size_t)assimpNode->mMeshes[0];

	const aiMesh* assimpMesh = assimpScene->mMeshes[meshId];
	const aiMaterial* assimpMaterial = assimpScene->mMaterials[assimpMesh->mMaterialIndex];

	model->m_mesh = resourceManager->AddResource<Mesh>(filepath, false, assimpMesh);
	model->m_material = resourceManager->AddResource<Material>(filepath, true, assimpMaterial);

	return model;
}

Model::~Model()
{
	m_mesh = nullptr;
	m_material = nullptr;
}
