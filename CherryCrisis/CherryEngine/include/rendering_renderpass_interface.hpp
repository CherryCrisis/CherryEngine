#pragma once

#include "callback.hpp"
#include "event.hpp"
#include "renderpass_interface.hpp"
#include "viewer.hpp"

struct Framebuffer;


class CCENGINE_API ARenderingRenderPass : public ARenderPass
{
protected:
	std::shared_ptr<CCCallback::ACallback<Viewer*&>> m_callExecute = nullptr;

public:
	virtual void InvalidatePass() { m_program = nullptr; m_callExecute = nullptr; }

	void PreDrawBind(ShaderProgram* program) { OnPreDrawBind(); }

	virtual void OnPreDrawBind() { }

	ARenderingRenderPass(const char* pipelineName, const char* vert, const char* frag)
		: ARenderPass(pipelineName, vert, frag)
	{
		if (m_program)
		{
			m_program->m_OnReloaded.Bind(&ARenderingRenderPass::PreDrawBind, this);
			m_program->m_OnDeleted.Bind(&ARenderingRenderPass::InvalidatePass, this);
		}
	}

	void CallOnExecute(Viewer* viewer)
	{
		if (m_callExecute)
			m_callExecute->Invoke(viewer);
	}

	virtual ~ARenderingRenderPass()
	{
		if (m_program)
		{
			m_program->m_OnReloaded.Unbind(&ARenderingRenderPass::PreDrawBind, this);
			m_program->m_OnDeleted.Unbind(&ARenderingRenderPass::InvalidatePass, this);
		}
	}
};