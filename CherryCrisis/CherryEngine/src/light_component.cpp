#include "pch.hpp"

#include "light_component.hpp"

#include "basic_renderpass.hpp"
#include "render_manager.hpp"

LightComponent::LightComponent(Entity& owner)
	: Behaviour(owner)
{
	RenderManager::GetInstance()->GenerateFromPipeline<BasicRenderPass>(&m_light);

	PopulateMetadatas();
}

LightComponent::~LightComponent()
{
	RenderManager::GetInstance()->RemoveFromPipeline<BasicRenderPass>(&m_light);
}

void LightComponent::PopulateMetadatas()
{
	m_metadatas.m_fields.push_back({ "position", &m_light.m_position });
	m_metadatas.m_fields.push_back({ "ambient",  &m_light.m_ambient });
	m_metadatas.m_fields.push_back({ "diffuse",  &m_light.m_diffuse });
	m_metadatas.m_fields.push_back({ "specular",  &m_light.m_specular });
	m_metadatas.m_fields.push_back({ "attenuation",  &m_light.m_attenuation });
}
