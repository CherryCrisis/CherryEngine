#include "pch.hpp"

#include "light_component.hpp"

#include "basic_renderpass.hpp"
#include "render_manager.hpp"

#include "maths.hpp"
#include "transform.hpp"

LightComponent::LightComponent()
{
	RenderManager::GetInstance()->SubscribeToPipeline<BasicRenderPass>(&m_light);

	PopulateMetadatas();
}

LightComponent::~LightComponent()
{
	RenderManager::GetInstance()->UnsubscribeToPipeline<BasicRenderPass>(&m_light);
}

void LightComponent::BindToSignals()
{
	GetHost().m_OnAwake.Bind(&LightComponent::Initialize, this);
}

void LightComponent::Initialize()
{
	m_transform = GetHost().GetBehaviour<Transform>();

	if (m_transform)
	{
		m_transform->m_onPositionChange.Bind(&LightComponent::ChangePosition, this);
		m_transform->m_onRotationChange.Bind(&LightComponent::ChangeRotation, this);
	}

	GetHost().m_OnAwake.Unbind(&LightComponent::Initialize, this);
}

void LightComponent::PopulateMetadatas()
{
	m_metadatas.SetField<CCMaths::Vector3>("ambient", m_light.m_ambient);
	m_metadatas.SetField<CCMaths::Vector3>("diffuse", m_light.m_diffuse);
	m_metadatas.SetField<CCMaths::Vector3>("specular", m_light.m_specular);
	m_metadatas.SetField<CCMaths::Vector3>("attenuation", m_light.m_attenuation);
}

void LightComponent::ChangePosition(const CCMaths::Vector3& position)
{
	m_light.m_position = position;
}

void LightComponent::ChangeRotation(const CCMaths::Vector3& rotation)
{
	//m_light.rotation = rotation;
}