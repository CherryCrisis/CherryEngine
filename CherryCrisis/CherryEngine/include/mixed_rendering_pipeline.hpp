#pragma once

#include "rendering_pipeline_interface.hpp"

class SkydomeRenderPass;
class PBRRPipeline;
class BasicRPipeline;

//PBR Rendering Pipeline
class CCENGINE_API MixedPipeline : public ARenderingPipeline
{
	SkydomeRenderPass*	m_skydomePass = nullptr;
	PBRRPipeline*		m_pbrPipeline = nullptr;
	BasicRPipeline*		m_basicPipeline = nullptr;

public:
	MixedPipeline();

	void Execute(Framebuffer& framebuffer, Viewer* viewer) override;
};
