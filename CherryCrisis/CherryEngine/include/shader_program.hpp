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

	static void Load(Ref<ShaderProgram> shaderProgram, const char* programName, const char* vxFilepath, const char* fgFilepath);
	static void Load(Ref<ShaderProgram> shaderProgram, const char* programName, const char* vxFilepath, const char* fgFilepath, const char* gmFilepath);							
	static void Load(Ref<ShaderProgram> shaderProgram, const char* programName, std::shared_ptr<Shader>& vx, std::shared_ptr<Shader>& fg);
	static void Load(Ref<ShaderProgram> shaderProgram, const char* programName, std::shared_ptr<Shader>& vx, std::shared_ptr<Shader>& fg, std::shared_ptr<Shader>& gm);
};
