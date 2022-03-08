#include "pch.hpp"

#include "material.hpp"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/cimport.h>

#include "resource_manager.hpp"

#include "texture.hpp"

Material::~Material()
{

}

Resource::Ref<Material> Material::Create(const char* filepath, const aiMaterial* assimpMaterial)
{
    ResourceManager* resourceManager = ResourceManager::GetInstance();

	Material* material = new Material(filepath);

	//Material Color
	aiColor3D color(0.f, 0.f, 0.f);
	if (AI_SUCCESS == assimpMaterial->Get(AI_MATKEY_COLOR_DIFFUSE, color))
		material->m_albedo = Vector3(color.r, color.g, color.b);

	//Material Texture
	{
		aiString texturePath;
		if (assimpMaterial->GetTexture(aiTextureType_AMBIENT, 0, &texturePath) == AI_SUCCESS)
		{
			material->textures["ambient"] = resourceManager->AddResource<Texture>(texturePath.C_Str(), true);
		}

		if (assimpMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &texturePath) == AI_SUCCESS)
		{
			// TODO: Remove this
			std::string path = "../assets/" + std::string(texturePath.C_Str());
			material->textures["albedo"] = resourceManager->AddResource<Texture>(path.c_str(), true);
		}
	}

	return Ref<Material>(material);
}
