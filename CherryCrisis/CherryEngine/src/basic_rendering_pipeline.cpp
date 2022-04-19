#include "pch.hpp"

#include "basic_rendering_pipeline.hpp"

#include "shadow_renderpass.hpp"
#include "skybox_renderpass.hpp"
#include "basic_renderpass.hpp"

#include "basic_postprocess_renderpass.hpp"

BasicRPipeline::BasicRPipeline()
{
	m_shadowPass = LoadSubpipeline<ShadowRenderPass>();
	m_basicPass = LoadSubpipeline<BasicRenderPass>();
	m_skyboxPass = LoadSubpipeline<SkyboxRenderPass>();

	m_postprocessPass = LoadSubpipeline<BasicPostProcessRenderPass>();
}

void BasicRPipeline::Execute(Framebuffer& fb, Camera& cam)
{
	m_shadowPass->CallOnExecute(fb, cam);
	m_basicPass->CallOnExecute(fb, cam);
	m_skyboxPass->CallOnExecute(fb, cam);

	//m_postprocessPass->CallOnExecute(fb);
}