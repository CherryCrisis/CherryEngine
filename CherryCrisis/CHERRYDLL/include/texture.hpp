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
    GPUTexture* gpuTexture = nullptr;

    static Resource* Create(const char* texturePath);

    ~Texture();
};