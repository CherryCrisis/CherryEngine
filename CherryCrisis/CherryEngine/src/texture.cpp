#include "pch.hpp"

#include "texture.hpp"

#include <stb_image.h>
#include <assimp/scene.h>
#include <assimp/texture.h>
#include <format>
#include <fstream>
#include <glad/gl.h>
#include <condition_variable>

#include "render_manager.hpp"
#include "model_loader.hpp"

#include "debug.hpp"



Texture::Texture(const char* texturePath)
    : Resource(texturePath)
{

}

void Texture::Delete()
{
    if (m_data)
        free(m_data);
}

void Texture::Load(std::shared_ptr<Texture> texture, bool flipTexture)
{
    //TODO: Replace to importTexture
    stbi_set_flip_vertically_on_load(flipTexture);

    texture->m_data = stbi_load(texture->GetFilepath(), &texture->m_width, &texture->m_height, NULL, STBI_rgb_alpha);

    if (!texture->m_data)
    {
        Debug* debug = Debug::GetInstance();
        debug->AddLog(ELogType::ERROR, std::format("{} {}", "Failed to load image", texture->GetFilepath()).c_str());
    }
}

void Texture::Load(std::shared_ptr<Texture> texture)
{
    unsigned char* data{};
    CCImporter::TextureHeader textureHeader{};

    if (!LoadFromCache(texture, &data, textureHeader))
    {
        CCImporter::ImportTexture(texture->GetFilepath(), &data, textureHeader);
    }

    if (!data)
    {
        Debug* debug = Debug::GetInstance();
        debug->AddLog(ELogType::ERROR, std::format("Failed to load image : {}", texture->GetFilepath()).c_str());
    }

    texture->m_data = (void*)std::move(data);
    texture->m_height = textureHeader.height;
    texture->m_width = textureHeader.width;
}

bool Texture::LoadFromCache(std::shared_ptr<Texture> texture, unsigned char** data, CCImporter::TextureHeader& textureHeader)
{
    FILE* file = nullptr;

    std::string texturePath(texture->GetFilepath());
    texturePath.erase(std::remove_if(texturePath.begin(), texturePath.end(), [](char c) {return c == '\\' || c == '/';}), texturePath.end());
    texturePath += CCImporter::cacheExtension;

    std::string fullTexturePath(CCImporter::cacheDirectory);
    fullTexturePath += texturePath;
    
    Debug* debug = Debug::GetInstance();
    if (fopen_s(&file, fullTexturePath.c_str(), "rb")) //rb = read in binary mode
        return false;

    fread(&textureHeader, sizeof(CCImporter::TextureHeader), 1, file);

    *data = new unsigned char[textureHeader.size];
    fread(&(*data)[0], sizeof(unsigned char), textureHeader.size, file);

    fclose(file);

    if (!*data)
    {
        debug->AddLog(ELogType::ERROR, std::format("{} {}", "Failed to load texture", texture->GetFilepath()).c_str());
        return false;
    }

    return true;
}

void Texture::Reload()
{
    /*if (m_data)
        free(m_data);

    stbi_set_flip_vertically_on_load(true);
    int nrComponents;

    m_data = stbi_load(GetFilepath(), &m_width, &m_height, &nrComponents, STBI_rgb_alpha);*/
}