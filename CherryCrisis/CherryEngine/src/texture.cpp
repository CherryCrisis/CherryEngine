#include "pch.hpp"

#include "texture.hpp"

#include <stb_image.h>
#include <stb_image_write.h>
#include <assimp/scene.h>
#include <assimp/texture.h>
#include <format>
#include <fstream>

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
    if (std::string(texture->GetFilepath()).compare("Assets/diffuse.jpg") == 0)
    {
        LoadFromCache(texture);
        return;
    }

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

void Texture::LoadFromCache(std::shared_ptr<Texture> texture)
{
    const char* cacheLocation = "Assets/Cache/";
    std::string filepath = std::format("{}{}.cache", cacheLocation,"test");
    FILE* file = nullptr;

    Debug* debug = Debug::GetInstance();

    if (fopen_s(&file, filepath.c_str(), "rb")) //rb = read in binary mode
    {
        debug->AddLog(ELogType::ERROR, std::format("{} {}", "Failed to open cache texture", filepath).c_str());
        return;
    }

    //struct Header
    //{
    //    int len;
    //    int height;
    //    int width;
    //};

    //Header header;
    //fread(&header, sizeof(Header), 1, file);

    std::vector<unsigned char> data;
    data.assign(26020852, 0);
    fread(&data[0], sizeof(unsigned char), 26020852, file);

    //texture->m_data = stbi_load_from_memory(data.data(), len, &texture->m_width, &texture->m_height, 0, STBI_rgb_alpha);

    texture->m_data = std::move(data.data());
    /*texture->m_len = header.len;
    texture->m_height = header.height;
    texture->m_width = header.width;*/

    texture->m_len = 26020852;
    texture->m_height = 4096;
    texture->m_width = 4096;

    /*26020852*/
    /*4096*/
    if (!texture->m_data)
    {
        debug->AddLog(ELogType::ERROR, std::format("{} {}", "Failed to load texture", texture->GetFilepath()).c_str());
        return;
    }
}

void Texture::SaveToCache()
{
    const char* cacheLocation = "Assets/Cache/";
    std::string filepath = std::format("{}{}.cache", cacheLocation, "test");
    FILE* file = nullptr;

    /*https://en.cppreference.com/w/c/io/fopen*/

    Debug* debug = Debug::GetInstance();
    if (fopen_s(&file, filepath.c_str(), "wb")) //w+b = write in binary mode
    {
        debug->AddLog(ELogType::ERROR, std::format("{} {}", "Failed to cache texture", filepath).c_str());
        return;
    }
    
    int stride = (m_width) * 4; //4 for RGBA
    int len;
    
    /*https://stackoverflow.com/questions/59671565/what-do-the-parameters-of-stb-png-to-mem-mean*/

    stbi_write_png_compression_level = 8;
    unsigned char* data = stbi_write_png_to_mem((const unsigned char*)m_data, stride, m_height, m_height, STBI_rgb_alpha, &len);
    
    if (!data)
    {
        debug->AddLog(ELogType::ERROR, std::format("{} [}", "data is null ", filepath).c_str());
        return;
    }
    
    //struct Header
    //{
    //    int len;
    //    int height;
    //    int width;
    //};
    //Header header{ .len = len, .height = m_height, .width = m_width };
    //fwrite(&header, sizeof(Header), 1, file);

    fwrite(&data[0], len, 1, file);
    fclose(file);
}

void Texture::Reload()
{
    if (m_data)
        stbi_image_free(m_data);

    stbi_set_flip_vertically_on_load(true);
    int nrComponents;

    m_data = stbi_load(GetFilepath(), &m_width, &m_height, &nrComponents, STBI_rgb_alpha);
}