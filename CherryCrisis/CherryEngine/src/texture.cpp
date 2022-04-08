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
    CCModelLoader::TextureHeader textureHeader{};

    if (!LoadFromCache(texture, &data, textureHeader))
    {
        CCModelLoader::ImportTexture(texture->GetFilepath(), &data, textureHeader);
    }

    if (!data)
    {
        Debug* debug = Debug::GetInstance();
        debug->AddLog(ELogType::ERROR, std::format("Failed to load image : {}", texture->GetFilepath()).c_str());
    }

    texture->m_data = (void*)std::move(data);
    texture->m_internalFormat = textureHeader.internalFormat;
    texture->m_compressedSize = textureHeader.compressedSize;
    texture->m_height = textureHeader.height;
    texture->m_width = textureHeader.width;
}

bool Texture::LoadFromCache(std::shared_ptr<Texture> texture, unsigned char** data, CCModelLoader::TextureHeader& textureHeader)
{
    FILE* file = nullptr;

    Debug* debug = Debug::GetInstance();

    std::string fullTexturePath("Assets/");
    fullTexturePath += texture->GetFilepath();

    if (fopen_s(&file, fullTexturePath.c_str(), "rb")) //rb = read in binary mode
    {
        debug->AddLog(ELogType::ERROR, std::format("{} {}", "Failed to open cache texture", fullTexturePath.c_str()).c_str());
        return false;
    }

    fread(&textureHeader, sizeof(CCModelLoader::TextureHeader), 1, file);

    *data = new unsigned char[textureHeader.compressedSize];
    fread(&(*data)[0], sizeof(unsigned char), textureHeader.compressedSize, file);


    if (!*data)
    {
        debug->AddLog(ELogType::ERROR, std::format("{} {}", "Failed to load texture", texture->GetFilepath()).c_str());
        return false;
    }

    return true;
}

void Texture::SaveToCache(std::mutex& mutex, std::condition_variable* condition)
{

    RenderManager::GetInstance();
    /*https://www.oldunreal.com/editing/s3tc/ARB_texture_compression.pdf*/

    GLuint tex;
    glGenTextures(1, &tex);
  

    glBindTexture(GL_TEXTURE_2D, tex);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_COMPRESSED_RGBA, m_width, m_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, m_data);

    int compressed;
    int internalFormat;
    int compressedSize;
    unsigned char* data = nullptr;

    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_COMPRESSED, &compressed);

    /* if the compression has been successful */
    if (compressed == GL_TRUE)
    {
        glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_INTERNAL_FORMAT, &internalFormat);
        glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_COMPRESSED_IMAGE_SIZE, &compressedSize);

        data = (unsigned char*)malloc(compressedSize);
        glGetCompressedTexImage(GL_TEXTURE_2D, 0, data);
    }

    Debug* debug = Debug::GetInstance();

    if (!data)
    {
        debug->AddLog(ELogType::ERROR, std::format("{} {}", "Failed to get compressed texture", m_filepath).c_str());
        return;
    }

    const char* cacheLocation = "Assets/Cache/";
    std::string filepath = std::format("{}{}.ccfile", cacheLocation, "test");
    FILE* file = nullptr;

    if (fopen_s(&file, filepath.c_str(), "wb")) //w+b = write in binary mode
    {
        debug->AddLog(ELogType::ERROR, std::format("{} {}", "Failed to cache texture", filepath).c_str());
        return;
    }

    struct Header
    {
        int internalFormat;
        int compressedSize;
        int height;
        int width;
    };

    Header header{ .internalFormat = internalFormat,.compressedSize = compressedSize, .height = m_height, .width = m_width };
    fwrite(&header, sizeof(Header), 1, file);

    fwrite(&data[0], compressedSize, 1, file);
    fclose(file);

    stbi_image_free(m_data);

    m_internalFormat = internalFormat;
    m_compressedSize = compressedSize;
    m_data = std::move(data);


    std::unique_lock<std::mutex> uniqueMutex(mutex);
    condition->notify_one();
}

void Texture::Reload()
{
    /*if (m_data)
        free(m_data);

    stbi_set_flip_vertically_on_load(true);
    int nrComponents;

    m_data = stbi_load(GetFilepath(), &m_width, &m_height, &nrComponents, STBI_rgb_alpha);*/
}