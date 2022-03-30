#pragma once

#include <memory>

#include "resource.hpp"

class Shader;

class ShaderProgram : public Resource<ShaderProgram>
{
public:
	ShaderProgram(const char* programName) : Resource(programName) {}

	unsigned int m_shaderProgram = 0u;

	~ShaderProgram();

	static void Load(std::shared_ptr<ShaderProgram> shaderProgram, const char* vxFilepath, const char* fgFilepath);
	static void Load(std::shared_ptr<ShaderProgram> shaderProgram, const char* vxFilepath, const char* fgFilepath, const char* gmFilepath);
	static void Load(std::shared_ptr<ShaderProgram> shaderProgram, std::shared_ptr<Shader>& vx, std::shared_ptr<Shader>& fg);
	static void Load(std::shared_ptr<ShaderProgram> shaderProgram, std::shared_ptr<Shader>& vx, std::shared_ptr<Shader>& fg, std::shared_ptr<Shader>& gm);
};
