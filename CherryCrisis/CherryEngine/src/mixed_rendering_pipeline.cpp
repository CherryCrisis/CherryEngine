#include "pch.hpp"

#include "mixed_rendering_pipeline.hpp"

//-- LIT --//
#include "shadow_renderpass.hpp"
#include "skybox_renderpass.hpp"
#include "basic_renderpass.hpp"
#include "portal_render_renderpass.hpp"
#include "bloom_renderpass.hpp"
#include "hdr_renderpass.hpp"
#include "basic_postprocess_renderpass.hpp"
#include "ui_renderpass.hpp"
#include "ui_text_renderpass.hpp"
#include "collider_renderpass.hpp"
#include "guizmo_renderpass.hpp"

//-- PBR --//
#include "environment_map_renderpass.hpp"
#include "skydome_renderpass.hpp"
#include "irradiance_map_renderpass.hpp"
#include "prefilter_map_renderpass.hpp"
#include "brdf_renderpass.hpp"
#include "pbr_renderpass.hpp"

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

	//-- PBR --//
	m_environmentMapPass = LoadSubpipeline<EnvironmentMapRenderPass>();
	m_irradianceMapPass = LoadSubpipeline<IrradianceMapRenderPass>();
	m_prefilterPass = LoadSubpipeline<PrefilterMapRenderPass>();
	m_brdfPass = LoadSubpipeline<BRDFRenderPass>();
	m_pbrPass = LoadSubpipeline<PBRRenderPass>();
	m_skydomePass = LoadSubpipeline<SkydomeRenderPass>();
}

void MixedPipeline::Execute(Framebuffer& framebuffer, Viewer* viewer)
{

	m_portalPass->CallOnExecute(framebuffer, viewer);
	m_shadowPass->CallOnExecute(framebuffer, viewer);

	m_basicPass->CallOnExecute(framebuffer, viewer);
	m_pbrPass->CallOnExecute(framebuffer, viewer);

	//m_bloomPass->CallOnExecute(framebuffer);
	//if (m_hdrPass->inBrightness = m_bloomPass->outBrightness)
	//	m_hdrPass->CallOnExecute(framebuffer);
	
	m_colliderPass->CallOnExecute(framebuffer, viewer);

	m_skydomePass->CallOnExecute(framebuffer, viewer);
	m_skyboxPass->CallOnExecute(framebuffer, viewer);
	
	m_guizmoPass->CallOnExecute(framebuffer, viewer);

	m_uiPass->CallOnExecute(framebuffer, viewer);
	m_uiTextPass->CallOnExecute(framebuffer, viewer);
	//m_postprocessPass->CallOnExecute(framebuffer);
}
