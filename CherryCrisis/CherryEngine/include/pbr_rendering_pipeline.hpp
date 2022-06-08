#pragma once

#include "rendering_pipeline_interface.hpp"

class BRDFRenderPass;
class EnvironmentMapRenderPass;
class IrradianceMapRenderPass;
class PBRRenderPass;
class PickingRenderPass;
class PrefilterMapRenderPass;
class SkydomeRenderPass;

//PBR Rendering Pipeline
class CCENGINE_API PBRRPipeline : public ARenderingPipeline
{
	EnvironmentMapRenderPass*	m_environmentMapPass	= nullptr;
	SkydomeRenderPass*			m_skydomePass			= nullptr;
	IrradianceMapRenderPass*	m_irradianceMapPass		= nullptr;
	PrefilterMapRenderPass*		m_prefilterPass			= nullptr;
	BRDFRenderPass*				m_brdfPass				= nullptr;
	PBRRenderPass*				m_pbrPass				= nullptr;
	PickingRenderPass*          m_pickingRenderpass     = nullptr;

public:
	PBRRPipeline();
	~PBRRPipeline();

	void Execute(Viewer* viewer, Framebuffer* framebuffer) override;
};
