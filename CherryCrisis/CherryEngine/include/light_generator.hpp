#pragma once

#include <glad/gl.h>

#include "light.hpp"

class CCENGINE_API LightGenerator
{
public:
	struct GPULightBasic : public GPULight
	{
		unsigned int index = 0u;
	};

	GLuint UBO = 0u;
};