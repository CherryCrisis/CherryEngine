#include "pch.hpp"

#include "shader_program.hpp"

#include <glad/gl.h>

#include "resource_manager.hpp"

#include "shader.hpp"

ShaderProgram::~ShaderProgram()
{
	glDeleteProgram(m_shaderProgram);
}

void ShaderProgram::Load(std::shared_ptr<ShaderProgram> shaderProgram, const char* vxFilepath, const char* fgFilepath)
{
	ResourceManager* resourceManager = ResourceManager::GetInstance();

	std::shared_ptr<Shader> vertexShader = resourceManager->AddResource<Shader>(vxFilepath, true, EShader::VERTEX);
	std::shared_ptr<Shader> fragmentShader;

	if (fgFilepath != "")
		fragmentShader = resourceManager->AddResource<Shader>(fgFilepath, true, EShader::FRAGMENT);

	Load(shaderProgram, vertexShader, fragmentShader);
}

void ShaderProgram::Load(std::shared_ptr<ShaderProgram> shaderProgram, const char* vxFilepath, const char* fgFilepath, const char* gmFilepath)
{
	ResourceManager* resourceManager = ResourceManager::GetInstance();

	std::shared_ptr<Shader> vertexShader = resourceManager->AddResource<Shader>(vxFilepath, true, EShader::VERTEX);
	std::shared_ptr<Shader> fragmentShader = resourceManager->AddResource<Shader>(vxFilepath, true, EShader::FRAGMENT);

	if (gmFilepath == "")
		Load(shaderProgram, vertexShader, fragmentShader);

	std::shared_ptr<Shader> geometryShader = resourceManager->AddResource<Shader>(vxFilepath, true, EShader::GEOMETRY);

	Load(shaderProgram, vertexShader, fragmentShader, geometryShader);
}

void ShaderProgram::Load(std::shared_ptr<ShaderProgram> shaderProgram, std::shared_ptr<Shader>& vx, std::shared_ptr<Shader>& fg)
{
	shaderProgram->m_shaderProgram = glCreateProgram();

	glAttachShader(shaderProgram->m_shaderProgram, vx->GetShaderID());

	if (fg)
		glAttachShader(shaderProgram->m_shaderProgram, fg->GetShaderID());

	glLinkProgram(shaderProgram->m_shaderProgram);

	GLint linkStatus;
	glGetProgramiv(shaderProgram->m_shaderProgram, GL_LINK_STATUS, &linkStatus);
	if (linkStatus == GL_FALSE)
	{
		char infolog[1024];
		glGetProgramInfoLog(shaderProgram->m_shaderProgram, (sizeof(infolog) / sizeof(infolog[0])), nullptr, infolog);
		fprintf(stderr, "Program link error: %s\n", infolog);
	}
}

void ShaderProgram::Load(std::shared_ptr<ShaderProgram> shaderProgram, std::shared_ptr<Shader>& vx, std::shared_ptr<Shader>& fg, std::shared_ptr<Shader>& gm)
{
	shaderProgram->m_shaderProgram = glCreateProgram();

	glAttachShader(shaderProgram->m_shaderProgram, vx->GetShaderID());

	if (gm.get() != nullptr)
		glAttachShader(shaderProgram->m_shaderProgram, gm->GetShaderID());

	if (fg.get() != nullptr)
		glAttachShader(shaderProgram->m_shaderProgram, fg->GetShaderID());

	glLinkProgram(shaderProgram->m_shaderProgram);

	GLint linkStatus;
	glGetProgramiv(shaderProgram->m_shaderProgram, GL_LINK_STATUS, &linkStatus);
	if (linkStatus == GL_FALSE)
	{
		char infolog[1024];
		glGetProgramInfoLog(shaderProgram->m_shaderProgram, (sizeof(infolog) / sizeof(infolog[0])), nullptr, infolog);
		fprintf(stderr, "Program link error: %s\n", infolog);
	}
}

