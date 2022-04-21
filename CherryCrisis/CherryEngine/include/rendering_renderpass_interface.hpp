#pragma once

#include "renderpass_interface.hpp"

#include "event.hpp"
#include "callback.hpp"

#include "viewer.hpp"

struct Framebuffer;

class ARenderingRenderPass : public ARenderPass
{
protected:
	std::shared_ptr<CCCallback::ACallback<Framebuffer&, Viewer*&>> m_callExecute = nullptr;

public:
	virtual void InvalidatePass() { m_callExecute = nullptr; }

	ARenderingRenderPass(const char* pipelineName, const char* vert, const char* frag)
		: ARenderPass(pipelineName, vert, frag)
	{
		if (m_program)
			m_program->m_OnDeleted.Bind(&ARenderingRenderPass::InvalidatePass, this);
	}

	void CallOnExecute(Framebuffer& framebuffer, Viewer* viewer)
	{
		if (m_callExecute)
			m_callExecute->Invoke(framebuffer, viewer);
	}

	virtual ~ARenderingRenderPass()
	{
		m_program->m_OnDeleted.Unbind(&ARenderingRenderPass::InvalidatePass, this);
	}
};