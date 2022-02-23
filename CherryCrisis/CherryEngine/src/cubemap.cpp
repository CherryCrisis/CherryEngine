#include "cubemap.hpp"

#include <stb_image.h>

Resource* Cubemap::Create(const char* texturePath, const char* textures[6])
{
    Cubemap* cubemap = new Cubemap(texturePath);

    stbi_set_flip_vertically_on_load(false);

    int channel;

    // Load and generate skybox faces
    for (unsigned int i = 0; i < 6; i++)
        cubemap->m_data[i] = stbi_load(textures[i], &cubemap->m_width, &cubemap->m_height, &channel, STBI_rgb_alpha);

    return cubemap;
}

Resource* Cubemap::Create(const char* cubemapName, const char* textureRight, const char* textureLeft, const char* textureTop, const char* textureBottom, const char* textureFront, const char* textureBack)
{
    const char* textures[6] = { textureRight, textureLeft, textureTop, textureBottom, textureFront, textureBack };
    return Cubemap::Create(cubemapName, textures);
}
