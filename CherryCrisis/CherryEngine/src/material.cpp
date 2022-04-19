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
			resourceManager->AddResource<Texture>((*materialArgs.m_texturesPath)[i].c_str(), true, true);

		material->textures.emplace((ETextureType)(*materialArgs.m_textureType)[i], texture);
	}
}

void Material::Reload(const MaterialArgs& materialArgs)
{
	ResourceManager* resourceManager = ResourceManager::GetInstance();

	m_ambient = materialArgs.m_materialHeader->m_ambient;
	m_diffuse = materialArgs.m_materialHeader->m_diffuse;
	m_specular = materialArgs.m_materialHeader->m_specular;
	m_emissive = materialArgs.m_materialHeader->m_emissive;

	m_shininess = materialArgs.m_materialHeader->m_shininess;

	textures.clear();

	for (unsigned int i = 0; i < materialArgs.m_materialHeader->m_texturesCount; ++i)
	{
		
		std::shared_ptr<Texture> texture = resourceManager->GetResource<Texture>((*materialArgs.m_texturesPath)[i].c_str());

		if (texture)
		{
			Resource<Texture>::ReloadResource(texture, true);
		}
		else
		{
			resourceManager->AddResource<Texture>((*materialArgs.m_texturesPath)[i].c_str(), false, true);
		}

		textures.emplace((ETextureType)(*materialArgs.m_textureType)[i], texture);
	}
}