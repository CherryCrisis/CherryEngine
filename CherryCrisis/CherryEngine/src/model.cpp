#include "pch.hpp"

#include "model.hpp"

#include "resource_manager.hpp"
#include "mesh.hpp"
#include "material.hpp"

void Model::Load(std::shared_ptr<Model> model)
{
	//ResourceManager* resourceManager = ResourceManager::GetInstance();
	//
	//size_t meshId = (size_t)assimpNode->mMeshes[0];
	//
	//const aiMesh* assimpMesh = assimpScene->mMeshes[meshId];
	//const aiMaterial* assimpMaterial = assimpScene->mMaterials[assimpMesh->mMaterialIndex];
	//
	//model->m_modelBasePath = modelBasePath;
	//
	//std::string meshPath = model->m_modelBasePath + std::string("/") + std::string(assimpMesh->mName.C_Str());
	//model->m_mesh = resourceManager->AddResource<Mesh>(meshPath.c_str(), true, assimpMesh);
	//
	//aiString name = assimpMaterial->GetName();
	//std::string materialPath = model->m_modelBasePath + std::string("/") + std::string(name.C_Str());
	//model->m_material = resourceManager->AddResource<Material>(materialPath.c_str(), true, assimpMaterial, assimpScene);

}

void Model::Load(std::shared_ptr<Model> model, std::shared_ptr<Mesh> mesh) 
{
	model->m_mesh = mesh;
}

void Model::Delete()
{
	m_mesh = nullptr;
	m_material = nullptr;
}

void Model::Reload()
{
	//ResourceManager* resourceManager = ResourceManager::GetInstance();
	//
	//size_t meshId = (size_t)assimpNode->mMeshes[0];
	//
	//const aiMesh* assimpMesh = assimpScene->mMeshes[meshId];
	//const aiMaterial* assimpMaterial = assimpScene->mMaterials[assimpMesh->mMaterialIndex];
	//
	//Resource<Mesh>::ReloadResource(m_mesh, assimpMesh);
	//Resource<Material>::ReloadResource(m_material, assimpMaterial);
}
