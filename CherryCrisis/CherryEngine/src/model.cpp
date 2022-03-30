#include "pch.hpp"

#include "model.hpp"

#include <assimp/mesh.h>
#include <assimp/material.h>
#include <assimp/scene.h>

#include "resource_manager.hpp"
#include "mesh.hpp"
#include "material.hpp"

void Model::Load(std::shared_ptr<Model> model, const aiScene* assimpScene, 
	const aiNode* assimpNode, const char* modelBasePath)
{
	ResourceManager* resourceManager = ResourceManager::GetInstance();

	size_t meshId = (size_t)assimpNode->mMeshes[0];

	const aiMesh* assimpMesh = assimpScene->mMeshes[meshId];
	const aiMaterial* assimpMaterial = assimpScene->mMaterials[assimpMesh->mMaterialIndex];
	
	std::string modelPath = model->GetFilepath();

	std::string meshPath = modelPath + std::string("/") + std::string(assimpMesh->mName.C_Str());
	model->m_mesh = resourceManager->AddResource<Mesh>(meshPath.c_str(), false, assimpMesh);

	aiString name = assimpMaterial->GetName();
	std::string materialPath = modelPath + std::string("/") + std::string(name.C_Str());
	model->m_material = resourceManager->AddResource<Material>(materialPath.c_str(), true, assimpMaterial);
	
	model->m_modelBasePath = std::string(modelBasePath);
}

Model::~Model()
{
	m_mesh = nullptr;
	m_material = nullptr;
}
