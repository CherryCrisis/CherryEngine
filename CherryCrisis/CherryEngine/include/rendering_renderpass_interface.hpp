#pragma once

#include "callback.hpp"
#include "event.hpp"
#include "renderpass_interface.hpp"
#include "viewer.hpp"

struct Framebuffer;


class CCENGINE_API ARenderingRenderPass : public ARenderPass
{
protected:
	std::shared_ptr<CCCallback::ACallback<Viewer*, Framebuffer*>> m_callExecute = nullptr;

public:
	virtual void InvalidatePass() { m_program = nullptr; m_callExecute = nullptr; }

	void PreDrawBind(ShaderProgram* program)
	{
		if (m_program)
			OnPreDrawBind();
	}

	virtual void OnPreDrawBind() {};

	void OnProgramLoaded(std::shared_ptr<ShaderProgram> program)
	{
		m_program->m_OnReloaded.Bind(&ARenderingRenderPass::PreDrawBind, this);
		m_program->m_OnDeleted.Bind(&ARenderingRenderPass::InvalidatePass, this);

		if (m_program)
			OnPreDrawBind();
	}

	ARenderingRenderPass(const char* pipelineName, const char* vert, const char* frag)
		: ARenderPass(pipelineName, vert, frag)
	{
		m_program->m_OnLoaded.Bind(&ARenderingRenderPass::OnProgramLoaded, this);
		if (m_program->GetResourceState() == EResourceState::LOADED)
			PreDrawBind(m_program.get());
	}

	void CallOnExecute(Viewer* viewer, Framebuffer* framebuffer)
	{
		if (m_callExecute)
			m_callExecute->Invoke(std::forward<Viewer*>(viewer), std::forward<Framebuffer*>(framebuffer));
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