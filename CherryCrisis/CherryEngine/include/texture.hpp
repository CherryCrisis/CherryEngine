#pragma once

#include "resource.hpp"

struct GPUTexture { };

class Texture : public Resource<Texture>
{
private:

    int     m_width = 0;
    int     m_height = 0;
    void*   m_data = nullptr;

public:
    Texture(const char* texturePath);

    GPUTexture* m_gpuTexture = nullptr;

    ~Texture() = default;

    int GetWidth()  { return m_width; }
    int GetHeight() { return m_height; }

    const void* GetData() { return m_data; }

    static void Load(std::shared_ptr<Texture> texture);
    void Delete() override;
    void Reload() override;
};