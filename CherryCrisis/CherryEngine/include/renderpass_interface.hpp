#pragma once

#include <memory>

#include "shader_program.hpp"
#include "resource_manager.hpp"

#include "event.hpp"
#include "callback.hpp"

#include "camera.hpp"

struct Framebuffer;

class ARenderPass
{
protected:
	std::shared_ptr<ShaderProgram> m_program;

	std::shared_ptr<CCCallback::ACallback<Framebuffer&, Camera&>> m_callExecute = nullptr;


public:
	virtual void InvalidatePass() { m_callExecute = nullptr; }

	ARenderPass(const char* pipelineName, const char* vert, const char* frag)
	{
		m_program = ResourceManager::GetInstance()->AddResource<ShaderProgram>(pipelineName, true, vert, frag);
		
		if (m_program)
			m_program->m_OnDeleted.Bind(&ARenderPass::InvalidatePass, this);
	}

	void CallOnExecute(Framebuffer& framebuffer, Camera& camera)
	{
		if (m_callExecute)
			m_callExecute->Invoke(framebuffer, camera);
	}

	virtual ~ARenderPass()
	{
		m_program->m_OnDeleted.Unbind(&ARenderPass::InvalidatePass, this);
	}
};