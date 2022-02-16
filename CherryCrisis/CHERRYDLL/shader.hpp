#pragma once

#include "resource.hpp"

enum class EShader
{
	FRAGMENT = 0,
	VERTEX	 = 1,
	GEOMETRY = 2,
};

class Shader : public Resource
{
private:
	Shader(const char* filepath) : Resource(filepath), m_shader(0) {}

	unsigned int m_shader;

public:

	static Resource* Create(const char* filepath, EShader shaderType);
};
