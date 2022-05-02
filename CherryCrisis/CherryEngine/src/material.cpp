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
	m_textures.clear();
}

void Material::Load(std::shared_ptr<Material> material)
{
	CCImporter::MaterialArgs materialArgs;

	if (!CCImporter::ImportMaterial(*material->GetFilesystemPath(), materialArgs))
	{
		CCImporter::SaveMaterial(material.get());
		return;
	}

	material->m_ambient = materialArgs.m_materialHeader.m_ambient;
	material->m_diffuse = materialArgs.m_materialHeader.m_diffuse;
	material->m_specular = materialArgs.m_materialHeader.m_specular;
	material->m_emissive = materialArgs.m_materialHeader.m_emissive;
	material->m_shininess = materialArgs.m_materialHeader.m_shininess;
	material->m_specularFactor = materialArgs.m_materialHeader.m_specularFactor;
	material->m_metallicFactor = materialArgs.m_materialHeader.m_metallicFactor;
	material->m_roughnessFactor = materialArgs.m_materialHeader.m_roughnessFactor;
	material->m_ao = materialArgs.m_materialHeader.m_ao;
	material->m_clearCoatFactor = materialArgs.m_materialHeader.m_clearCoatFactor;
	material->m_clearCoatRoughnessFactor = materialArgs.m_materialHeader.m_clearCoatRoughnessFactor;

    if (materialArgs.m_materialHeader.m_texturesCount)
    {
        ResourceManager* resourceManager = ResourceManager::GetInstance();

        for (unsigned int i = 0; i < materialArgs.m_materialHeader.m_texturesCount; ++i)
        {
            std::shared_ptr<Texture> texture =
                resourceManager->AddResource<Texture>(materialArgs.m_texturesPath[i].c_str(), true, true);

            material->m_textures.emplace(ETextureType(materialArgs.m_texturesType[i]), texture);
        }
    }
}

void Material::Reload()
{
	m_textures.clear();

	ResourceManager* resourceManager = ResourceManager::GetInstance();

	CCImporter::MaterialArgs materialArgs;

	if (!CCImporter::ImportMaterial(m_filepath, materialArgs))
	{
		CCImporter::SaveMaterial(this);
		return;
	}

	m_ambient = materialArgs.m_materialHeader.m_ambient;
	m_diffuse = materialArgs.m_materialHeader.m_diffuse;
	m_specular = materialArgs.m_materialHeader.m_specular;
	m_emissive = materialArgs.m_materialHeader.m_emissive;
	m_shininess = materialArgs.m_materialHeader.m_shininess;
	m_specularFactor = materialArgs.m_materialHeader.m_specularFactor;
	m_metallicFactor = materialArgs.m_materialHeader.m_metallicFactor;
	m_roughnessFactor = materialArgs.m_materialHeader.m_roughnessFactor;
	m_ao = materialArgs.m_materialHeader.m_ao;
	m_clearCoatFactor = materialArgs.m_materialHeader.m_clearCoatFactor;
	m_clearCoatRoughnessFactor = materialArgs.m_materialHeader.m_clearCoatRoughnessFactor;

	for (unsigned int i = 0; i < materialArgs.m_materialHeader.m_texturesCount; ++i)
	{
		std::shared_ptr<Texture> texture = resourceManager->GetResource<Texture>(materialArgs.m_texturesPath[i].c_str());

		if (!texture)
			resourceManager->AddResource<Texture>(materialArgs.m_texturesPath[i].c_str(), false, true);

		m_textures.emplace((ETextureType)materialArgs.m_texturesType[i], texture);
	}
}

void Material::SetTexture(ETextureType type, const std::shared_ptr<Texture>& newTexture) 
{
	m_textures[type] = newTexture;
	m_onTextureSet.Invoke(std::forward<std::shared_ptr<Texture>>(m_textures[type]));
}

void Material::SetTexture(ETextureType type, const char* filepath)
{
	std::shared_ptr<Texture> newTex = ResourceManager::GetInstance()->AddResource<Texture>(filepath, true);
	SetTexture(type, newTex);
}