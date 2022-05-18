#pragma once

#include "rendering_pipeline_interface.hpp"

#include "cherry_macros.hpp"

#include <vector>

class ShadowRenderPass;
class BasicRenderPass;
class SkyboxRenderPass;
class BloomRenderPass;
class PortalRenderPass;
class HDRRenderPass;
class ColliderRenderPass;
class PickingRenderPass;
class BasicPostProcessRenderPass;

class CCENGINE_API BasicRPipeline : public ARenderingPipeline
{
	ColliderRenderPass*			m_colliderPass = nullptr;
	PortalRenderPass*			m_portalPass = nullptr;
	ShadowRenderPass*			m_shadowPass = nullptr;
	BasicRenderPass*			m_basicPass = nullptr;
	SkyboxRenderPass*			m_skyboxPass = nullptr;
	BloomRenderPass*			m_bloomPass = nullptr;
	HDRRenderPass*				m_hdrPass = nullptr;
	BasicPostProcessRenderPass* m_postprocessPass = nullptr;
	PickingRenderPass*		    m_pickingRenderpass = nullptr;

public:
	BasicRPipeline();
	~BasicRPipeline();

	void Execute(Framebuffer& framebuffer, Viewer* viewer) override;
};