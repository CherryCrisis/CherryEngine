#pragma once

#include <array>

#include "resource.hpp"

struct GPUCubemap { };

class Cubemap : public Resource
{
private:
    int     m_width = 0;
    int     m_height = 0;
    std::array<void*, 6> m_data = { nullptr };

    Cubemap(const char* name)
        : Resource(name) { }

public:
    GPUCubemap* m_gpuCubemap = nullptr;

    int GetWidth()  { return m_width; }
    int GetHeight() { return m_height; }

    const std::array<void*, 6>& GetData() { return m_data; }

    static Ref<Cubemap> Create(const char* cubemapName, const char* textures[6]);
    static Ref<Cubemap> Create(const char* cubemapName, const char* textureRight, const char* textureLeft, const char* textureTop, const char* textureBottom, const char* textureFront, const char* textureBack);
};