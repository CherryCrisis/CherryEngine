#include <pch.hpp>

#include "mixed_rendering_pipeline.hpp"

//-- LIT --//
#include "basic_postprocess_renderpass.hpp"
#include "basic_renderpass.hpp"
#include "bloom_renderpass.hpp"
#include "collider_renderpass.hpp"
#include "guizmo_renderpass.hpp"
#include "hdr_renderpass.hpp"
#include "outline_renderpass.hpp"
#include "portal_render_renderpass.hpp"
#include "shadow_renderpass.hpp"
#include "skybox_renderpass.hpp"
#include "ui_renderpass.hpp"
#include "ui_text_renderpass.hpp"

//-- PBR --//
#include "brdf_renderpass.hpp"
#include "environment_map_renderpass.hpp"
#include "irradiance_map_renderpass.hpp"
#include "pbr_renderpass.hpp"
#include "picking_renderpass.hpp"
#include "prefilter_map_renderpass.hpp"
#include "skydome_renderpass.hpp"


MixedPipeline::MixedPipeline()
{
	//-- LIT --//
	m_colliderPass = LoadSubpipeline<ColliderRenderPass>();
	m_guizmoPass = LoadSubpipeline<GuizmoRenderPass>();
	m_portalPass = LoadSubpipeline<PortalRenderPass>();
	m_shadowPass = LoadSubpipeline<ShadowRenderPass>();
	m_basicPass = LoadSubpipeline<BasicRenderPass>();
	m_skyboxPass = LoadSubpipeline<SkyboxRenderPass>();
	m_bloomPass = LoadSubpipeline<BloomRenderPass>();
	m_hdrPass = LoadSubpipeline<HDRRenderPass>();
	m_postprocessPass = LoadSubpipeline<BasicPostProcessRenderPass>();
	m_uiPass = LoadSubpipeline<UIRenderPass>();
	m_uiTextPass = LoadSubpipeline<TextRenderPass>();
	m_outlinePass = LoadSubpipeline<OutlineRenderPass>();
	m_pickingRenderpass = LoadSubpipeline<PickingRenderPass>();

	//-- PBR --//
	m_environmentMapPass = LoadSubpipeline<EnvironmentMapRenderPass>();
	m_irradianceMapPass = LoadSubpipeline<IrradianceMapRenderPass>();
	m_prefilterPass = LoadSubpipeline<PrefilterMapRenderPass>();
	m_brdfPass = LoadSubpipeline<BRDFRenderPass>();
	m_pbrPass = LoadSubpipeline<PBRRenderPass>();
	m_skydomePass = LoadSubpipeline<SkydomeRenderPass>();
}

MixedPipeline::~MixedPipeline()
{
	//-- LIT --//
	delete m_colliderPass;
	delete m_guizmoPass;
	delete m_portalPass;
	delete m_shadowPass;
	delete m_basicPass;
	delete m_skyboxPass;
	delete m_bloomPass;
	delete m_hdrPass;
	delete m_postprocessPass;
	delete m_uiPass;
	delete m_uiTextPass;
	delete m_pickingRenderpass;
	delete m_outlinePass;
	
	//-- PBR --//
	delete m_environmentMapPass;
	delete m_irradianceMapPass;
	delete m_prefilterPass;
	delete m_brdfPass;
	delete m_pbrPass;
	delete m_skydomePass;
}

void MixedPipeline::Execute(Viewer* viewer)
{
	m_portalPass->CallOnExecute(viewer);
	m_shadowPass->CallOnExecute(viewer);

	m_basicPass->CallOnExecute(viewer);
	m_pbrPass->CallOnExecute(viewer);

	//m_bloomPass->CallOnExecute(viewer->framebuffer);
	//if (m_hdrPass->inBrightness = m_bloomPass->outBrightness)
	//	m_hdrPass->CallOnExecute(viewer->framebuffer);
	
	m_colliderPass->CallOnExecute(viewer);
	
	m_skyboxPass->CallOnExecute(viewer);
	m_skydomePass->CallOnExecute(viewer);
	
	m_guizmoPass->CallOnExecute(viewer);
	
	m_uiPass->CallOnExecute(viewer);
	m_uiTextPass->CallOnExecute(viewer);
	
	//m_outlinePass->CallOnExecute(viewer);
	//m_postprocessPass->CallOnExecute(viewer->framebuffer);
}
