#pragma once

#include "rendering_pipeline_interface.hpp"

#include "basic_rendering_pipeline.hpp"
#include "pbr_rendering_pipeline.hpp"


//-- LIT --//
class ShadowRenderPass;
class BasicRenderPass;
class SkyboxRenderPass;
class BloomRenderPass;
class PortalRenderPass;
class HDRRenderPass;
class ColliderRenderPass;

class BasicPostProcessRenderPass;

//-- PBR --//
class EnvironmentMapRenderPass;
class SkydomeRenderPass;
class IrradianceMapRenderPass;
class PrefilterMapRenderPass;
class BRDFRenderPass;
class PBRRenderPass;


//Mixed Pipeline, LIT, PBR
class CCENGINE_API MixedPipeline : public ARenderingPipeline
{
	//-- LIT --//
	ColliderRenderPass* m_colliderPass = nullptr;
	PortalRenderPass* m_portalPass = nullptr;
	ShadowRenderPass* m_shadowPass = nullptr;
	BasicRenderPass* m_basicPass = nullptr;
	SkyboxRenderPass* m_skyboxPass = nullptr;
	BloomRenderPass* m_bloomPass = nullptr;
	HDRRenderPass* m_hdrPass = nullptr;
	BasicPostProcessRenderPass* m_postprocessPass = nullptr;

	//-- PBR --//
	EnvironmentMapRenderPass* m_environmentMapPass = nullptr;
	SkydomeRenderPass* m_skydomePass = nullptr;
	IrradianceMapRenderPass* m_irradianceMapPass = nullptr;
	PrefilterMapRenderPass* m_prefilterPass = nullptr;
	BRDFRenderPass* m_brdfPass = nullptr;
	PBRRenderPass* m_pbrPass = nullptr;
	bool m_isGenerated = false;

public:
	MixedPipeline();

	void Execute(Framebuffer& framebuffer, Viewer* viewer) override;
};
