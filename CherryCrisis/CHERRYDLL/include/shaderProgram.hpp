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

	static Resource* Create(const char* programName, const char* vxFilepath, const char* fgFilepath);
	static Resource* Create(const char* programName, const char* vxFilepath, const char* fgFilepath, const char* gmFilepath);
	static Resource* Create(const char* programName, std::shared_ptr<Shader>& vx, std::shared_ptr<Shader>& fg);
	static Resource* Create(const char* programName, std::shared_ptr<Shader>& vx, std::shared_ptr<Shader>& fg, std::shared_ptr<Shader>& gm);
};
