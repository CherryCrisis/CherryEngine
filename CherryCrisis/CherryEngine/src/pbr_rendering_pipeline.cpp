#include "pch.hpp"

#include "pbr_rendering_pipeline.hpp"

#include "environment_map_renderpass.hpp"
#include "skydome_renderpass.hpp"
#include "irradiance_map_renderpass.hpp"
#include "prefilter_map_renderpass.hpp"
#include "brdf_renderpass.hpp"
#include "pbr_renderpass.hpp"

PBRRPipeline::PBRRPipeline()
{
	m_environmentMapPass = LoadSubpipeline<EnvironmentMapRenderPass>();
	m_irradianceMapPass = LoadSubpipeline<IrradianceMapRenderPass>();
	m_prefilterPass = LoadSubpipeline<PrefilterMapRenderPass>();
	m_brdfPass = LoadSubpipeline<BRDFRenderPass>();
	m_pbrPass = LoadSubpipeline<PBRRenderPass>();
	m_skydomePass = LoadSubpipeline<SkydomeRenderPass>();
}

void PBRRPipeline::Execute(Framebuffer& framebuffer, Viewer* viewer)
{
	if (!m_isGenerated)
	{
		//m_environmentMapPass->CallOnExecute(framebuffer, viewer);
		//m_irradianceMapPass->CallOnExecute(framebuffer, viewer);
		//m_prefilterPass->CallOnExecute(framebuffer, viewer);
		m_brdfPass->CallOnExecute(framebuffer, viewer);
		m_isGenerated = true;
	}

	m_pbrPass->CallOnExecute(framebuffer, viewer);
	m_skydomePass->CallOnExecute(framebuffer, viewer);
}