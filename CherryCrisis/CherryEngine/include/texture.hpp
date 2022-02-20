#pragma once

#include "resource.hpp"

struct GPUTexture { };

class Texture : public Resource
{
private:
    Texture(const char* texturePath);

    int     m_width;
    int     m_height;
    void*   m_data;

public:
    GPUTexture* m_gpuTexture = nullptr;

    ~Texture();

    int GetWidth()  { return m_width; }
    int GetHeight() { return m_height; }

    const void* GetData() { return m_data; }

    static Resource* Create(const char* texturePath);
};