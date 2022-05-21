#pragma once

#include <vector>

#include <cherry_macros.hpp>

#include "rendering_pipeline_interface.hpp"

class BasicPostProcessRenderPass;
class BasicRenderPass;
class BloomRenderPass;
class ColliderRenderPass;
class HDRRenderPass;
class PortalRenderPass;
class PickingRenderPass;
class ShadowRenderPass;
class SkyboxRenderPass;


class CCENGINE_API BasicRPipeline : public ARenderingPipeline
{
	BasicRenderPass*			m_basicPass			= nullptr;
	BloomRenderPass*			m_bloomPass			= nullptr;
	ColliderRenderPass*			m_colliderPass		= nullptr;
	PortalRenderPass*			m_portalPass		= nullptr;
	HDRRenderPass*				m_hdrPass			= nullptr;
	BasicPostProcessRenderPass* m_postprocessPass	= nullptr;
	PickingRenderPass*		    m_pickingRenderpass = nullptr;
	ShadowRenderPass*			m_shadowPass		= nullptr;
	SkyboxRenderPass*			m_skyboxPass		= nullptr;

public:
	BasicRPipeline();
	~BasicRPipeline();

	void Execute(Viewer* viewer) override;
};