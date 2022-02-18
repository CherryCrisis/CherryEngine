#pragma once

#include <memory>

#include "resource.hpp"

class Shader;

class ShaderProgram : public Resource
{
private:
	ShaderProgram(const char* programName) : Resource(programName), m_shaderProgram(0) {}

	unsigned int m_shaderProgram;
public:

	~ShaderProgram();

	static Resource* Create(const char* programName, const char* vxFilepath, const char* fgFilepath, const char* gmFilepath);
	static Resource* Create(const char* programName, std::shared_ptr<Shader>& vx, std::shared_ptr<Shader>& fg, std::shared_ptr<Shader>& gm);

};
