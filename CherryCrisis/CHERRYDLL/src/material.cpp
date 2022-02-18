#include "material.hpp"

#include <iostream>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/cimport.h>

#include "resourceManager.hpp"

#include "texture.hpp"

Resource* Material::Create(const char* modelFilepath, const aiMaterial* assimpMaterial)
{
    ResourceManager* resourceManager = ResourceManager::GetInstance();

	aiString name = assimpMaterial->GetName();
	std::string materialPath = modelFilepath + std::string("/")+ std::string(name.C_Str());

	Material* material = new Material(materialPath.c_str());

	//Material Color
	aiColor3D color(0.f, 0.f, 0.f);
	if (AI_SUCCESS == assimpMaterial->Get(AI_MATKEY_COLOR_AMBIENT, color))
		for (int i = 0; i < 3; ++i)
			material->m_albedo.data[i] = color[i];
	else 
		material->m_albedo = { 0.f };

	//Material Texture
    aiString texturePath;
	if (AI_SUCCESS == assimpMaterial->GetTexture(aiTextureType_AMBIENT, 0, &texturePath))
	{
		resourceManager->AddResource<Texture>(texturePath.C_Str(), true);
	}

	return material;
}