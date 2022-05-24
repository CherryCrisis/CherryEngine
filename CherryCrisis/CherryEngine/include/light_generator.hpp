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

	unsigned int m_lastIndex = 0;

	LightGenerator();

	virtual bool Generate(Light* toGenerate);

	static GLuint UBO;
	static GLuint UBOBindingPoint;
};