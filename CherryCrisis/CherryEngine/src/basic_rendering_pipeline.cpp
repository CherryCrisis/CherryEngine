#include "pch.hpp"

#include "basic_rendering_pipeline.hpp"

#include "shadow_renderpass.hpp"
#include "skybox_renderpass.hpp"
#include "basic_renderpass.hpp"
#include "portal_render_renderpass.hpp"
#include "collider_renderpass.hpp"

#include "bloom_renderpass.hpp"
#include "hdr_renderpass.hpp"
#include "basic_postprocess_renderpass.hpp"
#include "picking_renderpass.hpp"

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

void BasicRPipeline::Execute(Framebuffer& fb, Viewer* viewer)
{
	m_portalPass->CallOnExecute(fb, viewer);
	
	m_shadowPass->CallOnExecute(fb, viewer);
	m_basicPass->CallOnExecute(fb, viewer);

	m_bloomPass->CallOnExecute(fb);

	if (m_hdrPass->inBrightness = m_bloomPass->outBrightness)
		m_hdrPass->CallOnExecute(fb);
	
	m_colliderPass->CallOnExecute(fb, viewer);

	m_skyboxPass->CallOnExecute(fb, viewer);
	//m_postprocessPass->CallOnExecute(fb);
	
}
