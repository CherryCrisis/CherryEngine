#include "pch.hpp"

#include "texture.hpp"

#include <stb_image.h>
#include <assimp/scene.h>
#include <assimp/texture.h>


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

    if (texture->m_data)
    {
        //texture.gpu = renderer.CreateTexture(texture);
        //stbi_image_free(texture->m_data);
    }
    else
    {
        //TODO: Debug ErrorLog
        //std::cout << "Failed to load image. " << filename << std::endl;
    }
}

void Texture::Load(std::shared_ptr<Texture> texture, const aiTexture* assimpTexture, bool inutile)
{
    struct Test
    {
        unsigned int lenght;
        unsigned char* data;
    };

    Test test;

    if (assimpTexture->mHeight)
    {
        test.lenght = assimpTexture->mHeight * assimpTexture->mWidth * sizeof(assimpTexture->pcData);
    }
    else
    {
        test.lenght = assimpTexture->mWidth;
    }

    test.data = (unsigned char*)assimpTexture->pcData;

    texture->m_data = stbi_load_from_memory(test.data, test.lenght, &texture->m_width, &texture->m_height, 0, 0);

    //texture->m_data = reinterpret_cast<unsigned char*>(assimpTexture->pcData);
    //texture->m_width = assimpTexture->mWidth;
    //texture->m_height = assimpTexture->mHeight;
}

void Texture::Reload()
{
    if (m_data)
        stbi_image_free(m_data);

    stbi_set_flip_vertically_on_load(true);
    int nrComponents;

    m_data = stbi_load(GetFilepath(), &m_width, &m_height, &nrComponents, STBI_rgb_alpha);
}