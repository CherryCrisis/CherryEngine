#pragma once

#include "resource.hpp"

class Texture : public Resource
{
private:
    Texture(const char* texturePath);

    int     m_width;
    int     m_height;
    void*   m_data;

public:

    static Resource* Create(const char* texturePath);

    ~Texture();
};