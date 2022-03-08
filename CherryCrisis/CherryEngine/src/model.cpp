#include "pch.hpp"

#include "model.hpp"

#include <assimp/mesh.h>
#include <assimp/material.h>
#include <assimp/scene.h>

#include "resource_manager.hpp"
#include "mesh.hpp"
#include "material.hpp"

Resource::Ref<Model> Model::Create(const char* filepath, const aiScene* assimpScene, const aiNode* assimpNode)
{
	ResourceManager* resourceManager = ResourceManager::GetInstance();

	Model* model = new Model(filepath);

	size_t meshId = (size_t)assimpNode->mMeshes[0];

	const aiMesh* assimpMesh = assimpScene->mMeshes[meshId];
	const aiMaterial* assimpMaterial = assimpScene->mMaterials[assimpMesh->mMaterialIndex];
	
	std::string meshPath = filepath + std::string("/") + std::string(assimpMesh->mName.C_Str());
	model->m_mesh = resourceManager->AddResource<Mesh>(meshPath.c_str(), false, assimpMesh);

	aiString name = assimpMaterial->GetName();
	std::string materialPath = filepath + std::string("/") + std::string(name.C_Str());
	model->m_material = resourceManager->AddResource<Material>(materialPath.c_str(), true, assimpMaterial);

	return Ref<Model>(model);
}

Model::~Model()
{
	m_mesh = nullptr;
	m_material = nullptr;
}
