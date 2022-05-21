#include <pch.hpp>

#include "pbr_rendering_pipeline.hpp"

#include "brdf_renderpass.hpp"
#include "environment_map_renderpass.hpp"
#include "irradiance_map_renderpass.hpp"
#include "pbr_renderpass.hpp"
#include "picking_renderpass.hpp"
#include "prefilter_map_renderpass.hpp"
#include "skydome_renderpass.hpp"


PBRRPipeline::PBRRPipeline()
{
	m_environmentMapPass = LoadSubpipeline<EnvironmentMapRenderPass>();
	m_irradianceMapPass = LoadSubpipeline<IrradianceMapRenderPass>();
	m_prefilterPass = LoadSubpipeline<PrefilterMapRenderPass>();
	m_brdfPass = LoadSubpipeline<BRDFRenderPass>();
	m_pbrPass = LoadSubpipeline<PBRRenderPass>();
	m_skydomePass = LoadSubpipeline<SkydomeRenderPass>();
	m_pickingRenderpass = LoadSubpipeline<PickingRenderPass>();
}

PBRRPipeline::~PBRRPipeline()
{
	delete m_environmentMapPass;
	delete m_irradianceMapPass;
	delete m_prefilterPass;
	delete m_brdfPass;
	delete m_pbrPass;
	delete m_skydomePass;
	delete m_pickingRenderpass;
}

void PBRRPipeline::Execute(Viewer* viewer)
{
	m_pbrPass->CallOnExecute(viewer);
	m_skydomePass->CallOnExecute(viewer);
}