#include <pch.hpp>

#include "cubemap.hpp"

#include "resource_manager.hpp"

#include "texture.hpp"


void Cubemap::Load(std::shared_ptr<Cubemap> cubemap)
{
    std::array<std::string, 6> texturePaths;
    //if (!CCImporter::ImportCubemap(*cubemap->GetFilesystemPath(), texturePaths))
    //{
    //    //Save empty cubemap if doesn't exist
    //    CCImporter::SaveCubemap(cubemap.get());
    //}

    int width, height;
    bool ready = false; //Ready to use if all width/height of all textures are same

    ResourceManager* resourceManager = ResourceManager::GetInstance();
    for (unsigned int i = 0; i < 6; i++)
    {
        if (texturePaths[i].empty())
        {
            cubemap->m_textures[i] = nullptr;
            cubemap->m_data[i] = nullptr;
            ready = false;
            continue;
        }

        cubemap->m_textures[i] = resourceManager->AddResource<Texture>(texturePaths[i].c_str(), true, false, ETextureFormat::RGB);
        cubemap->m_data[i] = cubemap->m_textures[i]->GetData();

        if (i == 0)
        {
            width = cubemap->m_textures[i]->GetWidth();
            height = cubemap->m_textures[i]->GetHeight();
        }
        else
        {
            if (width != cubemap->m_textures[i]->GetWidth() || height != cubemap->m_textures[i]->GetHeight())
                ready = false;
        }
    }

        cubemap->m_width = ready ? width : 0;
        cubemap->m_height = ready ? height : 0;
        
}

void Cubemap::ClearData()
{
    for (unsigned int i = 0; i < 6; i++)
    {
        if (m_textures[i])
            m_textures[i]->ClearData();

        m_data[i] = nullptr;
    }
}

void Cubemap::Reload(bool saveOnly)
{
    //if (saveOnly)
    //{
    //    CCImporter::SaveCubemap(this);
    //    return;
    //}


    std::array<std::string, 6> texturePaths;
    //if (CCImporter::ImportCubemap(*GetFilesystemPath(), texturePaths))
    //{
    //    //Save empty cubemap if doesn't exist
    //    CCImporter::SaveCubemap(this);
    //}

    int width, height;
    bool ready = 0; //Ready to use if all width/height of all textures are same

    ResourceManager* resourceManager = ResourceManager::GetInstance();
    for (unsigned int i = 0; i < 6; i++)
    {
        if (texturePaths[i].empty())
        {
            m_textures[i] = nullptr;
            m_data[i] = nullptr;
            ready = false;
            continue;
        }

        std::shared_ptr<Texture> texture = resourceManager->GetResource<Texture>(texturePaths[i].c_str());

        if (!texture)
        {
            texture = resourceManager->AddResource<Texture>(texturePaths[i].c_str(), true, true, ETextureFormat::RGB);
        }
        
        m_textures[i] = texture;
        m_data[i] = texture->GetData();

        if (i == 0)
        {
            width = texture->GetWidth();
            height = texture->GetHeight();
        }
        else
        {
            if (width != texture->GetWidth() || height != texture->GetHeight())
                ready = false;
        }
    }

    m_width = ready ? width : 0;
    m_height = ready ? height : 0;

}
