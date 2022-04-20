#include "pch.hpp"

#include "light_component.hpp"

#include "render_manager.hpp"

#include "basic_renderpass.hpp"
#include "shadow_renderpass.hpp"

#include "maths.hpp"
#include "transform.hpp"

LightComponent::LightComponent()
{
	RenderManager::GetInstance()->SubscribeToPipeline<ShadowRenderPass>(&m_light);
	RenderManager::GetInstance()->SubscribeToPipeline<BasicRenderPass>(&m_light);

	PopulateMetadatas();
}

LightComponent::~LightComponent()
{
	RenderManager::GetInstance()->UnsubscribeToPipeline<ShadowRenderPass>(&m_light);
	RenderManager::GetInstance()->UnsubscribeToPipeline<BasicRenderPass>(&m_light);
}

void LightComponent::BindToSignals()
{
	GetHost().m_OnAwake.Bind(&LightComponent::Initialize, this);
}

void LightComponent::Initialize()
{
	m_transform = GetHost().GetOrAddBehaviour<Transform>();

	if (m_transform)
	{
		m_transform->m_onPositionChange.Bind(&LightComponent::ChangePosition, this);
		m_transform->m_onRotationChange.Bind(&LightComponent::ChangeRotation, this);
	}

	ChangePosition(m_transform->GetPosition());
	ChangeRotation(m_transform->GetRotation());

	GetHost().m_OnAwake.Unbind(&LightComponent::Initialize, this);
}

void LightComponent::PopulateMetadatas()
{
	m_metadatas.SetField<CCMaths::Vector3>("ambient", m_light.m_ambient);
	m_metadatas.SetField<CCMaths::Vector3>("diffuse", m_light.m_diffuse);
	m_metadatas.SetField<CCMaths::Vector3>("specular", m_light.m_specular);
	m_metadatas.SetField<CCMaths::Vector3>("attenuation", m_light.m_attenuation);
	m_metadatas.SetField<bool>("isPoint", m_light.m_isPoint);
}

void LightComponent::ChangePosition(const CCMaths::Vector3& position)
{
	m_light.m_position = position;

	m_light.m_lookAtMatrix = CCMaths::Matrix4::LookAt(m_light.m_position);

	// TODO: Add real ortho values
	m_light.m_lightSpace = CCMaths::Matrix4::Orthographic(-10.f, 10.f, -10.f, 10.f, -50.f, 20.f) * m_light.m_lookAtMatrix;
}

void LightComponent::ChangeRotation(const CCMaths::Vector3& rotation)
{
	//m_light.rotation = rotation;
}