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
        delete m_data;
}

void Texture::Load(std::shared_ptr<Texture> texture, bool flipTexture, ETextureFormat textureFormat)
{
    unsigned char* data{};
    CCImporter::TextureHeader textureHeader{};

    if (!LoadFromCache(texture, &data, textureHeader))
    {
       CCImporter::ImportTexture(*texture->GetFilesystemPath(), &data, textureHeader, flipTexture, textureFormat);
    }

    if (!data)
    {
        Debug* debug = Debug::GetInstance();
        debug->AddLog(ELogType::ERROR, std::format("Failed to load image : {}", texture->GetFilepath()).c_str());
    }

    texture->m_data = (void*)std::move(data);
    texture->m_height = textureHeader.height;
    texture->m_width = textureHeader.width;
    texture->m_size = textureHeader.size;
    texture->m_mipmapLevels = textureHeader.mipmapsLevel;
    texture->m_internalFormat = textureHeader.internalFormat;
}

#include "nvtt/nvtt.h"

bool Texture::LoadFromCache(std::shared_ptr<Texture> texture, unsigned char** data, CCImporter::TextureHeader& textureHeader)
{
    std::string fullTexturePath(CCImporter::cacheDirectory);
    fullTexturePath += texture->GetFilesystemPath()->filename().string();
    fullTexturePath += CCImporter::cacheExtension;

    FILE* file = nullptr;

    
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

void Texture::Reload(bool flipTexture, std::shared_ptr<Texture> texture)
{
    unsigned char* data{};
    CCImporter::TextureHeader textureHeader{};

    if (texture)
    {
        LoadFromCache(texture, &data, textureHeader);
    }
    else
    {
        CCImporter::ImportTexture(*GetFilesystemPath(), &data, textureHeader, flipTexture, ETextureFormat::RGBA);
    }

    if (!data)
    {
        Debug* debug = Debug::GetInstance();
        debug->AddLog(ELogType::ERROR, std::format("Failed to load image : {}", GetFilepath()).c_str());
    }

    m_data = (void*)std::move(data);
    m_height = textureHeader.height;
    m_width = textureHeader.width;
}