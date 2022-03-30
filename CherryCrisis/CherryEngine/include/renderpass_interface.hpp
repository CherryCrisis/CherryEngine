#pragma once

#include <memory>

#include "shader_program.hpp"
#include "resource_manager.hpp"

#include "event.hpp"
#include "callback.hpp"

class ARenderPass
{
protected:
	std::shared_ptr<ShaderProgram> m_program;

	std::unique_ptr<CCCallback::ACallback<const float&, const float&>> m_callExecute = nullptr;


public:
	virtual void InvalidatePass() { m_callExecute = nullptr; }

	ARenderPass(const char* pipelineName, const char* vert, const char* frag)
	{
		m_program = ResourceManager::GetInstance()->AddResource<ShaderProgram>(pipelineName, true, vert, frag);
		
		if (m_program)
			m_program->m_onDestroyed.Bind(&ARenderPass::InvalidatePass, this);
	}

	void CallOnExecute(float x, float y)
	{
		if (m_callExecute)
			m_callExecute->Invoke(x, y);
	}

	virtual ~ARenderPass()
	{
		m_program->m_onDestroyed.Unbind(&ARenderPass::InvalidatePass, this);
	}
};