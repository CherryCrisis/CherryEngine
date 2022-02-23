#pragma once

#include "resource.hpp"

struct GPUCubemap { };

class Cubemap : public Resource
{
private:
    Cubemap(const char* name)
        : Resource(name)
    {

    }

public:
    static Resource* Create(const char* texturePath);
};