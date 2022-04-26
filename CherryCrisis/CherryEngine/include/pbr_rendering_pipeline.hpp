#pragma once

#include "rendering_pipeline_interface.hpp"

class EnvironmentMapRenderPass;
class SkydomeRenderPass;
class IrradianceRenderPass;
class PrefilterRenderPass;
class BDRFRenderPass;

//PBR Rendering Pipeline
class CCENGINE_API PBRRPipeline : public ARenderingPipeline
{
	EnvironmentMapRenderPass* m_environmentMapPass	= nullptr;
	SkydomeRenderPass*		m_skyspherePass			= nullptr;
	IrradianceRenderPass*	m_irradiancePass		= nullptr;
	PrefilterRenderPass*	m_prefilterPass			= nullptr;
	BDRFRenderPass*			m_bdrf					= nullptr;

public:
	PBRRPipeline();

	void Execute(Framebuffer& framebuffer, Viewer* viewer) override;
};
