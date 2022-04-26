#pragma once

#include "rendering_pipeline_interface.hpp"

#include "cherry_macros.hpp"

#include <vector>

class ShadowRenderPass;
class BasicRenderPass;
//class SkyboxRenderPass;
class EnvironmentMapRenderPass;
class SkydomeRenderPass;
class BloomRenderPass;
class HDRRenderPass;

class BasicPostProcessRenderPass;

class CCENGINE_API BasicRPipeline : public ARenderingPipeline
{
	ShadowRenderPass*			m_shadowPass = nullptr;
	BasicRenderPass*			m_basicPass = nullptr;
	//SkyboxRenderPass*			m_skyboxPass = nullptr;
	EnvironmentMapRenderPass*	m_envMapPass = nullptr;
	SkydomeRenderPass*			m_skydomePass = nullptr;
	bool isGenerated = false;

	BloomRenderPass*			m_bloomPass = nullptr;
	HDRRenderPass*				m_hdrPass = nullptr;
	BasicPostProcessRenderPass* m_postprocessPass = nullptr;

public:
	BasicRPipeline();

	void Execute(Framebuffer& framebuffer, Viewer* viewer) override;
};