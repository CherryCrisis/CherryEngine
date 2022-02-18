#include "shaderProgram.hpp"

#include <glad/gl.h>
#include <assert.h>

#include "resourceManager.hpp"

#include "shader.hpp"

Resource* ShaderProgram::Create(const char* programName, const char* vxFilepath, const char* fgFilepath, const char* gmFilepath)
{
	ResourceManager* resourceManager = ResourceManager::GetInstance();

	assert(vxFilepath != "");
	std::shared_ptr<Shader> vertexShader = resourceManager->AddResource<Shader>(vxFilepath, true);
	std::shared_ptr<Shader> fragmentShader;
	std::shared_ptr<Shader> geometryShader;

	if (gmFilepath != "")
		std::shared_ptr<Shader> geometryShader = resourceManager->AddResource<Shader>(vxFilepath, true);

	if (fgFilepath != "")
		std::shared_ptr<Shader> fragmentShader = resourceManager->AddResource<Shader>(vxFilepath, true);

	Create(programName, vertexShader, fragmentShader, geometryShader);
}

Resource* ShaderProgram::Create(const char* programName, std::shared_ptr<Shader>& vx, std::shared_ptr<Shader>& fg, std::shared_ptr<Shader>& gm)
{
	ShaderProgram* shaderProgram = new ShaderProgram(programName);

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

	return shaderProgram;
}

ShaderProgram::~ShaderProgram()
{
	glDeleteProgram(m_shaderProgram);
}

