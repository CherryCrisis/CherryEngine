#pragma once

#include <memory>

#include "shader_program.hpp"
#include "resource_manager.hpp"

class ASubPipeline
{
protected:
	std::shared_ptr<ShaderProgram> m_program;

public:
	ASubPipeline(const char* pipelineName, const char* vert, const char* frag)
	{
		m_program = ResourceManager::GetInstance()->AddResource<ShaderProgram>(pipelineName, true, vert, frag);
	}

	virtual void Execute() = 0;

	virtual ~ASubPipeline() = default;
};