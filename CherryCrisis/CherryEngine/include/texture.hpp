#pragma once

#include "resource.hpp"

struct GPUTexture
{
    virtual ~GPUTexture() = default;
};

class CCENGINE_API Texture : public Resource<Texture>
{
private:
    int     m_width = 0;
    int     m_height = 0;
    void*   m_data = nullptr;

public:
    std::unique_ptr<GPUTexture> m_gpuTexture = nullptr;

    Texture(const char* texturePath);
    virtual ~Texture() = default;

    int GetWidth()  { return m_width; }
    int GetHeight() { return m_height; }

    void* GetData() { return m_data; }

    static void Load(std::shared_ptr<Texture> texture, bool flipTexture = true);
    void Delete() override;
    void Reload();
};