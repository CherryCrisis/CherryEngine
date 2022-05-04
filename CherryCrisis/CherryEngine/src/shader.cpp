#include "pch.hpp"

#include "shader.hpp"

#include <glad/gl.h>

void Shader::Delete()
{
	glDeleteShader(m_shaderID);
}

void Shader::Load(std::shared_ptr<Shader> shader)
{
	std::string extension = shader->GetFilesystemPath()->extension().string();
	if (!extension.compare(".frag"))
	{
		Load(shader, EShader::FRAGMENT);
	}
	else
	{
		Load(shader, EShader::VERTEX);
	}
}

void Shader::Load(std::shared_ptr<Shader> shader, EShader shaderType)
{
	shader->m_shaderType = shaderType;

	const std::string shaderStr = ParseShaderFromFile(*shader->GetFilesystemPath());
	const char* shaderCstr = shaderStr.c_str();

	shader->m_shaderID = CompileShader(shader->m_shaderType, &shaderCstr);
}

void Shader::Reload()
{
	glDeleteShader(m_shaderID);

	const std::string shaderStr = ParseShaderFromFile(*GetFilesystemPath());
	const char* shaderCstr = shaderStr.c_str();
	m_shaderID = CompileShader(m_shaderType, &shaderCstr);
}

std::string Shader::ParseShaderFromFile(std::filesystem::path& filepath)
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
