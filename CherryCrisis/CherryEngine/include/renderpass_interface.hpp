#pragma once

#include <memory>

#include "resource_manager.hpp"
#include "shader_program.hpp"

class CCENGINE_API ARenderPass
{
protected:
	std::shared_ptr<ShaderProgram> m_program;

public:
	ARenderPass(const char* pipelineName, const char* vert, const char* frag)
	{
		m_program = ResourceManager::GetInstance()->AddResource<ShaderProgram>(pipelineName, true, vert, frag);
	}

	~ARenderPass() = default;
};
