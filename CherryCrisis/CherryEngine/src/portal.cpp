#include "pch.hpp"

#include "portal.hpp"

#include "portal_render_renderpass.hpp"
#include "basic_rendering_pipeline.hpp"

void Portal::Link(Portal* toLinkTo)
{
	if (!toLinkTo)
		return;

	m_linkedPortal = toLinkTo;
	toLinkTo->m_linkedPortal = this;
}

void Portal::SubscribeToPipeline(ARenderingPipeline* pipeline)
{
	pipeline->SubscribeToPipeline<PortalRenderPass>(this);
}

void Portal::UnsubscribeToPipeline(ARenderingPipeline* pipeline)
{
	pipeline->UnsubscribeToPipeline<PortalRenderPass>(this);
}
