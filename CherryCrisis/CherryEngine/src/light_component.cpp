#include "pch.hpp"

#include "light_component.hpp"

#include "basic_renderpass.hpp"
#include "render_manager.hpp"

LightComponent::LightComponent()
{
	RenderManager::GetInstance()->SubscribeToPipeline<BasicRenderPass>(&m_light);

	PopulateMetadatas();
}

LightComponent::~LightComponent()
{
	RenderManager::GetInstance()->UnsubscribeToPipeline<BasicRenderPass>(&m_light);
}

void LightComponent::PopulateMetadatas()
{
	/*m_metadatas.m_fields["position"] = {"position", &m_light.m_position};
	m_metadatas.m_fields["ambient"] = { "ambient",  &m_light.m_ambient };
	m_metadatas.m_fields["diffuse"] = { "diffuse",  &m_light.m_diffuse };
	m_metadatas.m_fields["specular"] = { "specular",  &m_light.m_specular };
	m_metadatas.m_fields["attenuation"] = { "attenuation",  &m_light.m_attenuation };*/
	m_metadatas.SetField<CCMaths::Vector3>("position", m_light.m_position);
	m_metadatas.SetField<CCMaths::Vector3>("ambient", m_light.m_ambient);
	m_metadatas.SetField<CCMaths::Vector3>("diffuse", m_light.m_diffuse);
	m_metadatas.SetField<CCMaths::Vector3>("specular", m_light.m_specular);
	m_metadatas.SetField<CCMaths::Vector3>("attenuation", m_light.m_attenuation);
}
