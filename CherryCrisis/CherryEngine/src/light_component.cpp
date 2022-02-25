#include "pch.hpp"

#include "light_component.hpp"

#include "basic_renderpass.hpp"
#include "render_manager.hpp"

LightComponent::LightComponent()
{
	RenderManager::instance()->GenerateFromPipeline<BasicRenderPass>(&m_light);
}

LightComponent::~LightComponent()
{
	RenderManager::instance()->RemoveFromPipeline<BasicRenderPass>(&m_light);
}
