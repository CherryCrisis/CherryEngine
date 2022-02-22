#pragma once

#include "resource.hpp"

struct GPUCubemap { };

class Cubemap : public Resource
{
    static Resource* Create(const char* texturePath);

};