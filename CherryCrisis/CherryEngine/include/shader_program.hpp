#pragma once

#include <memory>

#include "resource.hpp"

class Shader;

class ShaderProgram : public Resource
{
private:
	ShaderProgram(const char* programName) : Resource(programName) {}

public:
	unsigned int m_shaderProgram = 0u;

	~ShaderProgram();

	static Ref<ShaderProgram> Create(const char* programName, const char* vxFilepath, const char* fgFilepath);
	static Ref<ShaderProgram> Create(const char* programName, const char* vxFilepath, const char* fgFilepath, const char* gmFilepath);
	static Ref<ShaderProgram> Create(const char* programName, std::shared_ptr<Shader>& vx, std::shared_ptr<Shader>& fg);
	static Ref<ShaderProgram> Create(const char* programName, std::shared_ptr<Shader>& vx, std::shared_ptr<Shader>& fg, std::shared_ptr<Shader>& gm);
};
