#include "pch.hpp"

#include "material.hpp"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/cimport.h>
#include <assimp/texture.h>

#include "resource_manager.hpp"

#include "texture.hpp"
#include "model_loader.hpp"

void Material::Delete()
{
	textures.clear();
}

void Material::Load(std::shared_ptr<Material> material, const MaterialArgs& materialArgs)
{
    ResourceManager* resourceManager = ResourceManager::GetInstance();

	material->m_ambient = materialArgs.m_materialHeader->m_ambient;
	material->m_diffuse = materialArgs.m_materialHeader->m_diffuse;
	material->m_specular = materialArgs.m_materialHeader->m_specular;
	material->m_emissive = materialArgs.m_materialHeader->m_emissive;

	material->m_shininess = materialArgs.m_materialHeader->m_shininess;

	for (unsigned int i = 0; i < materialArgs.m_materialHeader->m_texturesCount; ++i)
	{
		std::shared_ptr<Texture> texture =
			resourceManager->AddResource<Texture>((*materialArgs.m_texturesPath)[i].c_str(), true);

		material->textures.emplace((ETextureType)(*materialArgs.m_textureType)[i], texture);
	}
}

void Material::Reload(/*const aiMaterial* assimpMaterial*/)
{
	//ResourceManager* resourceManager = ResourceManager::GetInstance();

	////Material Color
	//aiColor3D color(0.f, 0.f, 0.f);
	//if (AI_SUCCESS == assimpMaterial->Get(AI_MATKEY_COLOR_DIFFUSE, color))
	//	m_albedo = Vector3(color.r, color.g, color.b);

	////Material Texture
	//{
	//	aiString texturePath;
	//	if (assimpMaterial->GetTexture(aiTextureType_AMBIENT, 0, &texturePath) == AI_SUCCESS)
	//	{
	//		Resource<Texture>::ReloadResource(textures["ambient"]);
	//	}

	//	if (assimpMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &texturePath) == AI_SUCCESS)
	//	{
	//		// TODO: Remove this
	//		std::string path = "Assets/" + std::string(texturePath.C_Str());
	//		Resource<Texture>::ReloadResource(textures["albedo"]);
	//	}
	//}
}