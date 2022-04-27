#include "pch.hpp"

#include "portal_render_renderpass.hpp"

#include "framebuffer.hpp"

#include "camera.hpp"
#include "model_renderer.hpp"
#include "transform.hpp"
#include "model.hpp"

#include "shadow_renderpass.hpp"
#include "portal.hpp"
#include "viewer.hpp"

PortalRenderPass::PortalRenderPass(const char* name)
	: ARenderingRenderPass(name, "Assets/basicShader.vert", "Assets/basicShader.frag")
{
	if (m_program)
		m_callExecute = CCCallback::BindCallback(&PortalRenderPass::Execute, this);
}

template <>
int PortalRenderPass::Subscribe(Portal* toGenerate)
{
	m_portals.insert(toGenerate);

	return 1;
}

template <>
void PortalRenderPass::Unsubscribe(Portal* toGenerate)
{
	m_portals.erase(toGenerate);
}

void PortalRenderPass::Execute(Framebuffer& framebuffer, Viewer*& viewer)
{
	if (!viewer)
		return;

	for (Portal* portal : m_portals)
		portal->m_pipeline->Execute(framebuffer, portal);
}
