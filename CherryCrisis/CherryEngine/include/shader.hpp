#pragma once

#include <string>

#include <cherry_macros.hpp>

#include "resource.hpp"


enum class EShader
{
	NOTHING,
	FRAGMENT = 0x8B30,
	VERTEX	 = 0x8B31,
	GEOMETRY = 0x8DD9
};

class CCENGINE_API Shader : public Resource<Shader>
{
private:
	unsigned int	m_shaderID;
	EShader			m_shaderType;

	static std::string ParseShaderFromFile(std::filesystem::path& filepath);
	static unsigned int CompileShader(EShader shaderType, const char** shaderStr);

public:
	Shader(const char* filepath) : Resource(filepath), m_shaderID(0), m_shaderType(EShader::NOTHING) {}
	~Shader() = default;

	static void Load(std::shared_ptr<Shader> shader);
	static void Load(std::shared_ptr<Shader> shader, EShader shaderType);
	void Delete() override;
	void Reload();

	const unsigned int GetShaderID() { return m_shaderID; }
};
