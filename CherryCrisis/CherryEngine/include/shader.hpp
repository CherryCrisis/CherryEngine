#pragma once

#include <string>

#include "resource.hpp"

enum class EShader
{
	FRAGMENT = 0x8B30,
	VERTEX	 = 0x8B31,
	GEOMETRY = 0x8DD9
};

class Shader : public Resource<Shader>
{
private:

	unsigned int m_shaderID;

	static std::string ParseShaderFromFile(const char* filepath);
	static unsigned int CompileShader(EShader shaderType, const char** shaderStr);

public:
	Shader(const char* filepath) : Resource(filepath), m_shaderID(0) {}

	~Shader();

	static void Load(std::shared_ptr<Shader> shader, EShader shaderType);

	const unsigned int GetShaderID() { return m_shaderID; }
};
