#include "pch.hpp"

#include "texture.hpp"

#include <stb_image.h>
#include <assimp/scene.h>
#include <assimp/texture.h>
#include <format>

#include "debug.hpp"


Texture::Texture(const char* texturePath)
    : Resource(texturePath)
{

}

void Texture::Delete()
{
    if (m_data)
        stbi_image_free(m_data);
}

void Texture::Load(std::shared_ptr<Texture> texture, bool flipTexture)
{
    stbi_set_flip_vertically_on_load(flipTexture);

    texture->m_data = stbi_load(texture->GetFilepath(), &texture->m_width, &texture->m_height, NULL, STBI_rgb_alpha);

    if (!texture->m_data)
    {
        Debug* debug = Debug::GetInstance();
        debug->AddLog(ELogType::ERROR, std::format("{} {}", "Failed to load image", texture->GetFilepath()).c_str());
    }
}

void Texture::Load(std::shared_ptr<Texture> texture, const aiTexture* assimpTexture)
{
    unsigned int len;

    if (assimpTexture->mHeight)
        len = assimpTexture->mHeight * assimpTexture->mWidth * sizeof(assimpTexture->pcData);
    else
        len = assimpTexture->mWidth;

    texture->m_data = stbi_load_from_memory((unsigned char*)assimpTexture->pcData, len, &texture->m_width, &texture->m_height, 0, STBI_rgb_alpha);

    if (!texture->m_data)
    {
        Debug* debug = Debug::GetInstance();
        debug->AddLog(ELogType::ERROR, std::format("{} {}", "Failed to load image", texture->GetFilepath()).c_str());
    }
}

void Texture::Reload()
{
    if (m_data)
        stbi_image_free(m_data);

    stbi_set_flip_vertically_on_load(true);
    int nrComponents;

    m_data = stbi_load(GetFilepath(), &m_width, &m_height, &nrComponents, STBI_rgb_alpha);
}