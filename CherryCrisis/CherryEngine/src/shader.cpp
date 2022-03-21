#include "pch.hpp"

#include "shader.hpp"

#include <glad/gl.h>

Shader::~Shader()
{
	glDeleteShader(m_shaderID);
}

void Shader::Load(std::shared_ptr<Shader> shader, EShader shaderType)
{
	const std::string shaderStr = ParseShaderFromFile(shader->GetFilepath());
	const char* shaderCstr = shaderStr.c_str();

	shader->m_shaderID = CompileShader(shaderType, &shaderCstr);
}

std::string Shader::ParseShaderFromFile(const char* filepath)
{
	std::ifstream ifs(filepath);

	std::string string_code;
	string_code.assign((std::istreambuf_iterator<char>(ifs)),
		(std::istreambuf_iterator<char>()));

	return string_code;
}

unsigned int Shader::CompileShader(EShader shaderType, const char** shaderStr)
{
	GLuint shader = glCreateShader((unsigned int)shaderType);

	glShaderSource(shader, (GLsizei)1, &shaderStr[0], nullptr);
	glCompileShader(shader);

	GLint compileStatus;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &compileStatus);
	if (compileStatus == GL_FALSE)
	{
		char infolog[1024];
		glGetShaderInfoLog(shader, (sizeof(infolog) / sizeof(infolog[0])), nullptr, infolog);
		fprintf(stderr, "Shader error: %s\n", infolog);
		//TODO: add debug log
	}

	return shader;
}
