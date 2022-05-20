#pragma once

#include "renderpass_interface.hpp"

#include "event.hpp"
#include "callback.hpp"

#include "viewer.hpp"

struct Framebuffer;

class CCENGINE_API ARenderingRenderPass : public ARenderPass
{
protected:
	std::shared_ptr<CCCallback::ACallback<Viewer*&>> m_callExecute = nullptr;

public:
	virtual void InvalidatePass() { m_callExecute = nullptr; }

	ARenderingRenderPass(const char* pipelineName, const char* vert, const char* frag)
		: ARenderPass(pipelineName, vert, frag)
	{
		if (m_program)
			m_program->m_OnDeleted.Bind(&ARenderingRenderPass::InvalidatePass, this);
	}

	void CallOnExecute(Viewer* viewer)
	{
		if (m_callExecute)
			m_callExecute->Invoke(viewer);
	}

	virtual ~ARenderingRenderPass()
	{
		m_program->m_OnDeleted.Unbind(&ARenderingRenderPass::InvalidatePass, this);
	}
};