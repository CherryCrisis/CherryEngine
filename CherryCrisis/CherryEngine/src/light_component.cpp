#include "pch.hpp"

#include "light_component.hpp"

#include "basic_renderpass.hpp"
#include "render_manager.hpp"

LightComponent::LightComponent(Entity& owner)
	: Behaviour(owner)
{
	RenderManager::GetInstance()->GenerateFromPipeline<BasicRenderPass>(&m_light);
}

LightComponent::~LightComponent()
{
	RenderManager::GetInstance()->RemoveFromPipeline<BasicRenderPass>(&m_light);
}
