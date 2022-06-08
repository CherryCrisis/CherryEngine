#include <pch.hpp>

#include "texture.hpp"

#include <assimp/scene.h>
#include <assimp/texture.h>
#include <condition_variable>
#include <format>
#include <fstream>
#include <glad/gl.h>
#include <stb_image.h>

#include "debug.hpp"
#include "render_manager.hpp"

#include "model_loader.hpp"


Texture::Texture(const char* texturePath)
    : Resource(texturePath)
{

}

Texture::~Texture()
{
    if (GetResourceState() == EResourceState::DESTROYED)
        Delete();
}

void Texture::Delete()
{
    ClearData();
    m_gpuTexture2D.reset();
}

void Texture::Load(std::shared_ptr<Texture> texture, void* data, int width, int height, ETextureFormat textureFormat, ETextureFilter textureMinFilter, ETextureFilter textureMagFilter, ETextureWrap textureWrapS, ETextureWrap textureWrapT, ETextureWrap textureWrapR)
{
    texture->m_data = std::move(data);
    texture->m_height = height;
    texture->m_width = width;
    texture->m_internalFormat = textureFormat;
    texture->m_minFilter = textureMinFilter;
    texture->m_magFilter = textureMagFilter;
    texture->m_wrapS = textureWrapS;
    texture->m_wrapT = textureWrapT;
    texture->m_wrapR = textureWrapR;
    texture->m_stackAllocated = true;
}

void Texture::Load(std::shared_ptr<Texture> texture, bool flipTexture, ETextureFormat textureFormat, ETextureFilter textureMinFilter, ETextureFilter textureMagFilter, ETextureWrap textureWrapS, ETextureWrap textureWrapT, ETextureWrap textureWrapR)
{
    unsigned char* data{};
    CCImporter::TextureHeader textureHeader{};

    if (!LoadFromCache(texture, &data, textureHeader))
    {
       CCImporter::ImportTexture(*texture->GetFilesystemPath(), &data, textureHeader, flipTexture, textureFormat, ETextureSurface::TEXTURE_2D, textureWrapS, textureWrapT, textureWrapR, textureMinFilter, textureMagFilter);
    }

    if (!data)
    {
        Debug* debug = Debug::GetInstance();
        debug->AddLog(ELogType::ERROR, std::format("Failed to load image : {}", texture->GetFilepath()).c_str());
    }

    texture->m_data = data;
    texture->m_height = textureHeader.height;
    texture->m_width = textureHeader.width;
    texture->m_size = textureHeader.size;
    texture->m_mipmapLevels = textureHeader.mipmapsLevel;
    texture->m_internalFormat = textureHeader.internalFormat;
    texture->m_wrapS = textureHeader.wrapS;
    texture->m_wrapT = textureHeader.wrapT;
    texture->m_wrapR = textureHeader.wrapR;
    texture->m_minFilter = textureHeader.minFilter;
    texture->m_magFilter = textureHeader.magFilter;
    texture->m_surface = textureHeader.surface;
    texture->m_blockSize = textureHeader.blockSize;
    texture->m_flipped = textureHeader.flipped;
    texture->m_stackAllocated = false;
}

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

void Texture::ClearData()
{
    if (m_data && !m_stackAllocated)
        delete m_data;

    m_data = nullptr;
}

void Texture::Reload(bool flipTexture)
{
    unsigned char* data{};
    CCImporter::TextureHeader textureHeader{};

    CCImporter::ImportTexture(*GetFilesystemPath(), &data, textureHeader, flipTexture, m_internalFormat, m_surface, m_wrapS, m_wrapT, m_wrapR, m_minFilter, m_magFilter, false);

    if (!data)
    {
        Debug* debug = Debug::GetInstance();
        debug->AddLog(ELogType::ERROR, std::format("Failed to load image : {}", GetFilepath()).c_str());
    }

    m_data = (void*)std::move(data);
    m_height = textureHeader.height;
    m_width = textureHeader.width;
    m_size = textureHeader.size;
    m_mipmapLevels = textureHeader.mipmapsLevel;
    m_internalFormat = textureHeader.internalFormat;
    m_surface = textureHeader.surface;
    m_blockSize = textureHeader.blockSize;
    m_flipped = textureHeader.flipped;
}
