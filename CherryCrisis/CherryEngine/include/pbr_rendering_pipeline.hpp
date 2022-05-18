#pragma once

#include "rendering_pipeline_interface.hpp"

class EnvironmentMapRenderPass;
class SkydomeRenderPass;
class IrradianceMapRenderPass;
class PrefilterMapRenderPass;
class BRDFRenderPass;
class PBRRenderPass;
class PickingRenderPass;

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

	void Execute(Framebuffer& framebuffer, Viewer* viewer) override;
};
