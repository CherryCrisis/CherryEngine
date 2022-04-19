#pragma once

#include "rendering_pipeline_interface.hpp"

#include <vector>

class ShadowRenderPass;
class BasicRenderPass;
class SkyboxRenderPass;

class BasicPostProcessRenderPass;

class BasicRPipeline : public ARenderingPipeline
{
	ShadowRenderPass*	m_shadowPass = nullptr;
	BasicRenderPass*	m_basicPass = nullptr;
	SkyboxRenderPass* m_skyboxPass = nullptr;
	BasicPostProcessRenderPass*	m_postprocessPass = nullptr;

public:
	BasicRPipeline();

	void Execute(Framebuffer& framebuffer, Camera& camera) override;
};