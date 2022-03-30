#pragma once

#include <memory>

#include "resource.hpp"

class Shader;

class ShaderProgram : public Resource<ShaderProgram>
{
private:
	void CreateProgram();

public:
	ShaderProgram(const char* programName) : Resource(programName) {}

	std::vector<std::shared_ptr<Shader>> m_shaders;
	unsigned int m_shaderProgram = 0u;

	~ShaderProgram() = default;

	static void Load(std::shared_ptr<ShaderProgram> shaderProgram, const char* vxFilepath, const char* fgFilepath);
	static void Load(std::shared_ptr<ShaderProgram> shaderProgram, const char* vxFilepath, const char* fgFilepath, const char* gmFilepath);
	static void Load(std::shared_ptr<ShaderProgram> shaderProgram, std::shared_ptr<Shader>& vx, std::shared_ptr<Shader>& fg);
	static void Load(std::shared_ptr<ShaderProgram> shaderProgram, std::shared_ptr<Shader>& vx, std::shared_ptr<Shader>& fg, std::shared_ptr<Shader>& gm);

	void Delete() override;
	void Reload() override;
};
