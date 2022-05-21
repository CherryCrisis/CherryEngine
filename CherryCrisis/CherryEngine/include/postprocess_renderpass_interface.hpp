#pragma once

#include "callback.hpp"
#include "camera.hpp"
#include "event.hpp"
#include "renderpass_interface.hpp"

struct Framebuffer;


class APostProcessRenderPass : public ARenderPass
{
protected:
	std::shared_ptr<CCCallback::ACallback<Framebuffer&>> m_callExecute = nullptr;

public:
	virtual void InvalidatePass() { m_callExecute = nullptr; }

	APostProcessRenderPass(const char* pipelineName, const char* vert, const char* frag)
		: ARenderPass(pipelineName, vert, frag)
	{
		if (m_program)
			m_program->m_OnDeleted.Bind(&APostProcessRenderPass::InvalidatePass, this);
	}

	void CallOnExecute(Framebuffer& framebuffer)
	{
		if (m_callExecute)
			m_callExecute->Invoke(framebuffer);
	}

	virtual ~APostProcessRenderPass()
	{
		m_program->m_OnDeleted.Unbind(&APostProcessRenderPass::InvalidatePass, this);
	}
};