#pragma once

#include <array>

#include <cherry_macros.hpp>

#include "resource.hpp"

struct GPUCubemap { };
class Texture;


class CCENGINE_API Cubemap : public Resource<Cubemap>
{
private:
    int     m_width = 0;
    int     m_height = 0;
    std::array<void*, 6> m_data = { nullptr };

public:
    std::array<std::shared_ptr<Texture>, 6> m_textures;
    std::unique_ptr<GPUCubemap> m_gpuCubemap = nullptr;

    Cubemap(const char* name)
        : Resource(name) { }
    ~Cubemap() = default;

    int GetWidth()  { return m_width; }
    int GetHeight() { return m_height; }

    const std::array<void*, 6>& GetData() { return m_data; }

    static void Load(std::shared_ptr<Cubemap> cubemap);

    void ClearData();
    void Delete() override {};

    void Reload(bool saveOnly = false);
};