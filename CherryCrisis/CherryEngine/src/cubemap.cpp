#include "pch.hpp"

#include "cubemap.hpp"

#include <stb_image.h>

#include "resource_manager.hpp"

#include "texture.hpp"

void Cubemap::Delete()
{
   // for (int i = 0; i < 6; i++)
   //     stbi_image_free(m_data[i]);
}

void Cubemap::Load(std::shared_ptr<Cubemap> cubemap, const char* textures[6], const char* relativePath)
{
    //stbi_set_flip_vertically_on_load(false);

    //int channel;

    // Load and generate skybox faces
    //for (unsigned int i = 0; i < 6; i++)
    //    cubemap->m_data[i] = stbi_load(textures[i], &cubemap->m_width, &cubemap->m_height, &channel, STBI_rgb_alpha);

    ResourceManager* resourceManager = ResourceManager::GetInstance();
    for (unsigned int i = 0; i < 6; i++)
    {
        cubemap->m_textures[i] = resourceManager->AddResource<Texture>(textures[i], true, false, relativePath);
        cubemap->m_data[i] = cubemap->m_textures[i]->GetData();
    }

    cubemap->m_width = cubemap->m_textures[0]->GetWidth();
    cubemap->m_height= cubemap->m_textures[0]->GetHeight();
        
}

void Cubemap::Load(std::shared_ptr<Cubemap> cubemap, const char* textureRight, const char* textureLeft, const char* textureTop, const char* textureBottom, const char* textureFront, const char* textureBack, const char* relativePath)
{
    const char* textures[6] = { textureRight, textureLeft, textureTop, textureBottom, textureFront, textureBack };
    Cubemap::Load(cubemap, textures, relativePath);
}
