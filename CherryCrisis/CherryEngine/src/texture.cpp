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

//void Texture::Load(std::shared_ptr<Texture> texture, bool flipTexture)
//{
//    Load(texture, flipTexture, "");
//}

void Texture::Load(std::shared_ptr<Texture> texture, bool flipTexture)
{
    unsigned char* data{};
    CCImporter::TextureHeader textureHeader{};

    if (!LoadFromCache(texture, &data, textureHeader))
    {
        CCImporter::ImportTexture(*texture->GetFilesystemPath(), &data, textureHeader, flipTexture);
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

    std::string fullTexturePath(CCImporter::cacheDirectory);
    fullTexturePath += texture->GetFilesystemPath()->filename().string();
    fullTexturePath += CCImporter::cacheExtension;
    
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