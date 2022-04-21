#include "pch.hpp"

#include "basic_rendering_pipeline.hpp"

#include "shadow_renderpass.hpp"
#include "skybox_renderpass.hpp"
#include "basic_renderpass.hpp"

#include "bloom_renderpass.hpp"
#include "basic_postprocess_renderpass.hpp"

BasicRPipeline::BasicRPipeline()
{
	m_shadowPass = LoadSubpipeline<ShadowRenderPass>();
	m_basicPass = LoadSubpipeline<BasicRenderPass>();
	m_skyboxPass = LoadSubpipeline<SkyboxRenderPass>();

	m_bloomPass = LoadSubpipeline<BloomRenderPass>();

	m_postprocessPass = LoadSubpipeline<BasicPostProcessRenderPass>();
}

void BasicRPipeline::Execute(Framebuffer& fb, Viewer* viewer)
{
	m_shadowPass->CallOnExecute(fb, viewer);
	m_basicPass->CallOnExecute(fb, viewer);
	m_skyboxPass->CallOnExecute(fb, viewer);

	m_bloomPass->CallOnExecute(fb);

	//m_postprocessPass->CallOnExecute(fb);
}