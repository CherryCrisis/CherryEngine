#include "pch.hpp"

#include "texture.hpp"

#include <stb_image.h>

Texture::Texture(const char* texturePath)
    : Resource(texturePath)
{

}

Texture::~Texture()
{
    if (m_resourceState.load() != EResourceState::DESTROYED)
        Delete();
}

void Texture::Delete()
{
    if (m_data)
        stbi_image_free(m_data);
}


void Texture::Load(std::shared_ptr<Texture> texture)
{
    stbi_set_flip_vertically_on_load(true);
    int nrComponents;

    texture->m_data = stbi_load(texture->GetFilepath(), &texture->m_width, &texture->m_height, &nrComponents, STBI_rgb_alpha);

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