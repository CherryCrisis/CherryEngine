#pragma once

#include "basic_rendering_pipeline.hpp"
#include "pbr_rendering_pipeline.hpp"
#include "rendering_pipeline_interface.hpp"

//-- LIT --//
class BasicPostProcessRenderPass;
class BasicRenderPass;
class BloomRenderPass;
class ColliderRenderPass;
class GuizmoRenderPass;
class HDRRenderPass;
class PortalRenderPass;
class PickingRenderPass;
class ShadowRenderPass;
class SkyboxRenderPass;
class TextRenderPass;
class UIRenderPass;

//-- PBR --//
class BRDFRenderPass;
class EnvironmentMapRenderPass;
class IrradianceMapRenderPass;
class PBRRenderPass;
class PrefilterMapRenderPass;
class SkydomeRenderPass;


//Mixed Pipeline, LIT, PBR
class CCENGINE_API MixedPipeline : public ARenderingPipeline
{
	//-- LIT --//
	ColliderRenderPass*			m_colliderPass		= nullptr;
	GuizmoRenderPass*			m_guizmoPass		= nullptr;
	PortalRenderPass*			m_portalPass		= nullptr;
	ShadowRenderPass*			m_shadowPass		= nullptr;
	BasicRenderPass*			m_basicPass			= nullptr;
	SkyboxRenderPass*			m_skyboxPass		= nullptr;
	BloomRenderPass*			m_bloomPass			= nullptr;
	HDRRenderPass*				m_hdrPass			= nullptr;
	BasicPostProcessRenderPass* m_postprocessPass	= nullptr;
	UIRenderPass*				m_uiPass			= nullptr;
	TextRenderPass*				m_uiTextPass		= nullptr;
	PickingRenderPass*			m_pickingRenderpass = nullptr;

	//-- PBR --//
	EnvironmentMapRenderPass*	m_environmentMapPass = nullptr;
	SkydomeRenderPass*			m_skydomePass		 = nullptr;
	IrradianceMapRenderPass*	m_irradianceMapPass  = nullptr;
	PrefilterMapRenderPass*		m_prefilterPass		 = nullptr;
	BRDFRenderPass*				m_brdfPass			 = nullptr;
	PBRRenderPass*				m_pbrPass			 = nullptr;

public:
	MixedPipeline();
	~MixedPipeline();

	void Execute(Viewer* viewer) override;
};
