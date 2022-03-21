#pragma once

#include <array>

#include "resource.hpp"

struct GPUCubemap { };

class Cubemap : public Resource<Cubemap>
{
private:
    int     m_width = 0;
    int     m_height = 0;
    std::array<void*, 6> m_data = { nullptr };



public:
    GPUCubemap* m_gpuCubemap = nullptr;

    Cubemap(const char* name)
        : Resource(name) { }
    ~Cubemap();

    int GetWidth()  { return m_width; }
    int GetHeight() { return m_height; }

    const std::array<void*, 6>& GetData() { return m_data; }

    static void Load(std::shared_ptr<Cubemap> cubemap, const char* textures[6]);
    static void Load(std::shared_ptr<Cubemap> cubemap, const char* textureRight, const char* textureLeft, const char* textureTop, const char* textureBottom, const char* textureFront, const char* textureBack);
};