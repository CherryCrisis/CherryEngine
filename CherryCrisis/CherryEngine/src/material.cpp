#include "material.hpp"

#include <iostream>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/cimport.h>

#include "resourceManager.hpp"

#include "texture.hpp"

Material::~Material()
{

}

Resource* Material::Create(const char* modelFilepath, const aiMaterial* assimpMaterial)
{
    ResourceManager* resourceManager = ResourceManager::GetInstance();

	aiString name = assimpMaterial->GetName();
	std::string materialPath = modelFilepath + std::string("/") + std::string(name.C_Str());

	Material* material = new Material(materialPath.c_str());

	//Material Color
	aiColor3D color(0.f, 0.f, 0.f);
	if (AI_SUCCESS == assimpMaterial->Get(AI_MATKEY_COLOR_AMBIENT, color))
		for (int i = 0; i < 3; ++i)
			material->m_albedo.data[i] = color[i];
	else 
		material->m_albedo = { 0.f };
	//Material Texture
	{
		aiString ambientTexturePath;
		if (AI_SUCCESS == assimpMaterial->GetTexture(aiTextureType_AMBIENT, 0, &ambientTexturePath))
		{
			material->ambientTexture = resourceManager->AddResource<Texture>(ambientTexturePath.C_Str(), true);
		}
		aiString albedoTexturePath;
		if (AI_SUCCESS == assimpMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &albedoTexturePath))
		{
			// TODO: Remove this
			std::string path = "../Assets/" + std::string(albedoTexturePath.C_Str());
			material->albedoTexture = resourceManager->AddResource<Texture>(path.c_str(), true);
		}
	}

	return material;
}
