#include "pch.hpp"

#include "cubemap.hpp"

#include <stb_image.h>

Cubemap::~Cubemap()
{
    for (int i = 0; i < 6; i++)
        stbi_image_free(m_data[i]);
}

void Cubemap::Load(Ref<Cubemap> cubemap, const char* texturePath, const char* textures[6])
{
    stbi_set_flip_vertically_on_load(false);

    int channel;

    // Load and generate skybox faces
    for (unsigned int i = 0; i < 6; i++)
        cubemap->m_data[i] = stbi_load(textures[i], &cubemap->m_width, &cubemap->m_height, &channel, STBI_rgb_alpha);
}

void Cubemap::Load(Ref<Cubemap> cubemap, const char* cubemapName, const char* textureRight, const char* textureLeft, const char* textureTop, const char* textureBottom, const char* textureFront, const char* textureBack)
{
    const char* textures[6] = { textureRight, textureLeft, textureTop, textureBottom, textureFront, textureBack };
    Cubemap::Load(cubemap, cubemapName, textures);
}
