#include <pch.hpp>

#include "shader_program.hpp"

#include <glad/gl.h>

#include "resource_manager.hpp"

#include "shader.hpp"


void ShaderProgram::Delete()
{
	glDeleteProgram(m_shaderProgram);

	Invalidate();
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

	shaderProgram->m_shaders.push_back(vx);

	if (fg.get() != nullptr)
		shaderProgram->m_shaders.push_back(fg);

	shaderProgram->InitializeProgram();
}

void ShaderProgram::Load(std::shared_ptr<ShaderProgram> shaderProgram, std::shared_ptr<Shader>& vx, std::shared_ptr<Shader>& fg, std::shared_ptr<Shader>& gm)
{
	shaderProgram->m_shaderProgram = glCreateProgram();

	shaderProgram->m_shaders.push_back(vx);

	if (gm.get() != nullptr)
		shaderProgram->m_shaders.push_back(gm);

	if (fg.get() != nullptr)
		shaderProgram->m_shaders.push_back(fg);

	shaderProgram->InitializeProgram();
}

void ShaderProgram::InitializeProgram()
{
	for (auto& shader : m_shaders)
	{
		shader->m_OnDeleted.Bind(&ShaderProgram::Invalidate, this);
		shader->m_OnReloaded.Bind(&ShaderProgram::OnReload, this);
	}

	CreateProgram();
}

void ShaderProgram::CreateProgram()
{
	for (auto& shader : m_shaders)
		glAttachShader(m_shaderProgram, shader->GetShaderID());

	glLinkProgram(m_shaderProgram);

	GLint linkStatus;
	glGetProgramiv(m_shaderProgram, GL_LINK_STATUS, &linkStatus);
	if (linkStatus == GL_FALSE)
	{
		char infolog[1024];
		glGetProgramInfoLog(m_shaderProgram, (sizeof(infolog) / sizeof(infolog[0])), nullptr, infolog);
		fprintf(stderr, "Program link error: %s\n", infolog);
		//TODO: Add debug
	}
}

void ShaderProgram::Reload()
{
	glDeleteProgram(m_shaderProgram); 
	m_shaderProgram = glCreateProgram();

	CreateProgram();
}

void ShaderProgram::OnReload(Shader* shader)
{
	Resource<ShaderProgram>::ReloadResource(this);
}

void ShaderProgram::Invalidate()
{
	for (auto& shader : m_shaders)
		shader->m_OnReloaded.Unbind(&ShaderProgram::OnReload, this);

	m_shaders.clear();
}
