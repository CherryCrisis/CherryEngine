#include "pch.hpp"

#include "spheremap.hpp"

#include "resource_manager.hpp"

#include "texture.hpp"

Spheremap::~Spheremap()
{
    if (GetResourceState() != EResourceState::DESTROYED)
        Delete();
}


void Spheremap::Load(std::shared_ptr<Spheremap> spheremap)
{
    std::string texturePath;
    if (!CCImporter::ImportSpheremap(*spheremap->GetFilesystemPath(), texturePath))
    {
        CCImporter::SaveSpheremap(spheremap.get());
        return;
    }

    if (!texturePath.empty())
    {
        ResourceManager* resourceManager = ResourceManager::GetInstance();
        spheremap->m_texture = resourceManager->AddResource<Texture>(texturePath.c_str(), true, true, ETextureFormat::RGB);
    }
}

int Spheremap::GetWidth()
{
    return m_texture->GetWidth();
}

int Spheremap::GetHeight()
{
    return m_texture->GetHeight();
}

ETextureFormat Spheremap::GetInternalFormat()
{
    return m_texture->GetInternalFormat();
}

const void* Spheremap::GetData()
{
    return m_texture->GetData();
}

void Spheremap::ClearData()
{
    if (m_texture)
        m_texture->ClearData();
}
void Spheremap::Delete()
{
    ClearData();
}

void Spheremap::Reload(bool saveOnly)
{
    if (saveOnly)
    {
        CCImporter::SaveSpheremap(this);
        return;
    }

    std::string texturePath;
    if (!CCImporter::ImportSpheremap(*GetFilesystemPath(), texturePath))
    {
        CCImporter::SaveSpheremap(this);
        return;
    }

    if (!texturePath.empty())
    {
        ResourceManager* resourceManager = ResourceManager::GetInstance();
        std::shared_ptr<Texture> texture = resourceManager->GetResource<Texture>(texturePath.c_str());
        
        if (!texture)
        {
            texture = resourceManager->AddResource<Texture>(texturePath.c_str(), true, true, ETextureFormat::RGB);
        }

        m_texture = texture;
    }
}
