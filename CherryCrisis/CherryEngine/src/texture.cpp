#include "pch.hpp"

#include "texture.hpp"

#include <stb_image.h>
#include <assimp/scene.h>
#include <assimp/texture.h>
#include <format>
#include <fstream>
#include <glad/gl.h>

#include "render_manager.hpp"

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
    if (std::string(texture->GetFilepath()).compare("Assets/diffuse.jpg") == 0)
        if (LoadFromCache(texture))
            return;

    stbi_set_flip_vertically_on_load(flipTexture);

    texture->m_data = stbi_load(texture->GetFilepath(), &texture->m_width, &texture->m_height, NULL, STBI_rgb_alpha);

    if (!texture->m_data)
    {
        Debug* debug = Debug::GetInstance();
        debug->AddLog(ELogType::ERROR, std::format("{} {}", "Failed to load image", texture->GetFilepath()).c_str());
    }

    if (std::string(texture->GetFilepath()).compare("Assets/diffuse.jpg") == 0)
        texture->SaveToCache();
}

void Texture::Load(std::shared_ptr<Texture> texture, const aiTexture* assimpTexture)
{
    //LoadFromCache(texture);
    //return;

    unsigned int len;

    if (assimpTexture->mHeight)
        len = assimpTexture->mHeight * assimpTexture->mWidth * sizeof(assimpTexture->pcData);
    else
        len = assimpTexture->mWidth;

    texture->m_data = stbi_load_from_memory((unsigned char*)assimpTexture->pcData, len, &texture->m_width, &texture->m_height, 0, STBI_rgb_alpha);

    if (!texture->m_data)
    {
        Debug* debug = Debug::GetInstance();
        debug->AddLog(ELogType::ERROR, std::format("{} {}", "Failed to load texture", texture->GetFilepath()).c_str());
    }

    //texture->SaveToCache();
}

bool Texture::LoadFromCache(std::shared_ptr<Texture> texture)
{
    const char* cacheLocation = "Assets/Cache/";
    std::string filepath = std::format("{}{}.ccfile", cacheLocation,"test");
    FILE* file = nullptr;

    Debug* debug = Debug::GetInstance();

    if (fopen_s(&file, filepath.c_str(), "rb")) //rb = read in binary mode
    {
        debug->AddLog(ELogType::ERROR, std::format("{} {}", "Failed to open cache texture", filepath).c_str());
        return false;
    }

    struct Header
    {
        int internalFormat;
        int compressedSize;
        int height;
        int width;
    };

    Header header;
    fread(&header, sizeof(Header), 1, file);

    unsigned char* data = (unsigned char*)malloc(header.compressedSize);

    fread(&data[0], sizeof(unsigned char), header.compressedSize, file);

    texture->m_data = std::move(data);

    texture->m_height = header.height;
    texture->m_width = header.width;
    texture->m_compressedSize = header.compressedSize;
    texture->m_internalFormat = header.internalFormat;

    if (!texture->m_data)
    {
        debug->AddLog(ELogType::ERROR, std::format("{} {}", "Failed to load texture", texture->GetFilepath()).c_str());
        return false;
    }

    return true;
}

void Texture::SaveToCache()
{
    RenderManager::GetInstance();
    /*https://www.oldunreal.com/editing/s3tc/ARB_texture_compression.pdf*/

    GLuint tex;

    glGenTextures(1, &tex);

    /*https://en.cppreference.com/w/c/io/fopen*/


    //glGenTextures(1, &tex);
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

    m_internalFormat = internalFormat;
    m_compressedSize = compressedSize;

    stbi_image_free(m_data);
    m_data = std::move(data);
}

void Texture::Reload()
{
    /*if (m_data)
        free(m_data);

    stbi_set_flip_vertically_on_load(true);
    int nrComponents;

    m_data = stbi_load(GetFilepath(), &m_width, &m_height, &nrComponents, STBI_rgb_alpha);*/
}