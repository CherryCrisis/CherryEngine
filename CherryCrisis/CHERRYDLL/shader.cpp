#include "shader.hpp"

Resource* Shader::Create(const char* filepath, EShader shaderType)
{
	Shader* shader = new Shader(filepath);
	return shader;
}