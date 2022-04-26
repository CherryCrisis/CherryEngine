#include "pch.hpp"

#include "basic_rendering_pipeline.hpp"

#include "shadow_renderpass.hpp"
#include "skybox_renderpass.hpp"
#include "basic_renderpass.hpp"

#include "bloom_renderpass.hpp"
#include "hdr_renderpass.hpp"
#include "basic_postprocess_renderpass.hpp"

#include "environment_map_renderpass.hpp"
#include "skydome_renderpass.hpp"

BasicRPipeline::BasicRPipeline()
{
	m_shadowPass = LoadSubpipeline<ShadowRenderPass>();
	m_basicPass = LoadSubpipeline<BasicRenderPass>();
	//m_skyboxPass = LoadSubpipeline<SkyboxRenderPass>();
	m_envMapPass = LoadSubpipeline<EnvironmentMapRenderPass>();
	m_skydomePass = LoadSubpipeline<SkydomeRenderPass>();

	m_bloomPass = LoadSubpipeline<BloomRenderPass>();
	m_hdrPass = LoadSubpipeline<HDRRenderPass>();

	m_postprocessPass = LoadSubpipeline<BasicPostProcessRenderPass>();
}

void BasicRPipeline::Execute(Framebuffer& fb, Viewer* viewer)
{
	m_shadowPass->CallOnExecute(fb, viewer);
	m_basicPass->CallOnExecute(fb, viewer);

	m_bloomPass->CallOnExecute(fb);

	if (m_hdrPass->inBrightness = m_bloomPass->outBrightness)
		m_hdrPass->CallOnExecute(fb);

	//if (!isGenerated)
	//{
		m_envMapPass->Execute();
		isGenerated = true;
	//}

	//m_skyboxPass->CallOnExecute(fb, viewer);
	m_skydomePass->CallOnExecute(fb, viewer);
	//m_postprocessPass->CallOnExecute(fb);
}