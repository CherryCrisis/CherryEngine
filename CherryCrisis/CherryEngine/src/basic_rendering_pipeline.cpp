#include <pch.hpp>

#include "basic_rendering_pipeline.hpp"

#include "basic_postprocess_renderpass.hpp"
#include "basic_renderpass.hpp"
#include "bloom_renderpass.hpp"
#include "collider_renderpass.hpp"
#include "hdr_renderpass.hpp"
#include "picking_renderpass.hpp"
#include "portal_render_renderpass.hpp"
#include "shadow_renderpass.hpp"
#include "skybox_renderpass.hpp"


BasicRPipeline::BasicRPipeline()
{
	m_portalPass = LoadSubpipeline<PortalRenderPass>();
	m_shadowPass = LoadSubpipeline<ShadowRenderPass>();
	m_basicPass = LoadSubpipeline<BasicRenderPass>();
	m_skyboxPass = LoadSubpipeline<SkyboxRenderPass>();
	m_bloomPass = LoadSubpipeline<BloomRenderPass>();
	m_hdrPass = LoadSubpipeline<HDRRenderPass>();
	m_colliderPass = LoadSubpipeline<ColliderRenderPass>();
	m_pickingRenderpass = LoadSubpipeline<PickingRenderPass>();

	m_postprocessPass = LoadSubpipeline<BasicPostProcessRenderPass>();
}

BasicRPipeline::~BasicRPipeline()
{
	delete m_portalPass;
	delete m_shadowPass;
	delete m_basicPass;
	delete m_skyboxPass;
	delete m_bloomPass;
	delete m_hdrPass;
	delete m_colliderPass;
	delete m_pickingRenderpass;
	delete m_postprocessPass;
}

void BasicRPipeline::Execute(Viewer* viewer, Framebuffer* framebuffer)
{
	m_portalPass->CallOnExecute(viewer, framebuffer);

	m_shadowPass->CallOnExecute(viewer, framebuffer);
	m_basicPass->CallOnExecute(viewer, framebuffer);

	m_bloomPass->CallOnExecute(*viewer->m_framebuffer);

	if (m_hdrPass->inBrightness = m_bloomPass->outBrightness)
		m_hdrPass->CallOnExecute(*viewer->m_framebuffer);
	
	m_colliderPass->CallOnExecute(viewer, framebuffer);
	m_skyboxPass->CallOnExecute(viewer, framebuffer);
	//m_postprocessPass->CallOnExecute(viewer->m_framebuffer);
	
}
