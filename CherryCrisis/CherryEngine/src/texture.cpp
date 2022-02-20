#include "texture.hpp"

#include <stb_image.h>

Texture::Texture(const char* texturePath)
    : Resource(texturePath), m_width(0), m_height(0), m_data(0)
{

}

Texture::~Texture()
{

}


Resource* Texture::Create(const char* texturePath)
{
	Texture* texture = new Texture(texturePath);

    stbi_set_flip_vertically_on_load(true);
    int nrComponents;

    texture->m_data = stbi_load(texturePath, &texture->m_width, &texture->m_height, &nrComponents, STBI_rgb_alpha);

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

	return texture;
}