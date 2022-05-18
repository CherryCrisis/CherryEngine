#include "pch.hpp"

#include "light_component.hpp"

#include "render_manager.hpp"

#include "basic_renderpass.hpp"
#include "shadow_renderpass.hpp"
#include "pbr_renderpass.hpp"
#include "guizmo_renderpass.hpp"

#include "maths.hpp"
#include "transform.hpp"

#include "cell.hpp"
#include "entity.hpp"

#include "camera_component.hpp"

LightComponent::LightComponent()
{
	PopulateMetadatas();
	SubscribeGuizmo();
}

LightComponent::LightComponent(CCUUID& id) : Behaviour(id)
{
	PopulateMetadatas();
	SubscribeGuizmo();
}

LightComponent::~LightComponent()
{
	GetHost().m_cell->RemoveRenderer(this);

	if (m_transform)
	{
		m_transform->m_onPositionChange.Unbind(&LightComponent::ChangePosition, this);
		m_transform->m_onRotationChange.Unbind(&LightComponent::ChangeRotation, this);
		m_transform->m_OnDestroy.Unbind(&LightComponent::InvalidateTransform, this);
	}

	UnsubscribeGuizmo();
}

void LightComponent::BindToSignals()
{
	GetHost().m_OnAwake.Bind(&LightComponent::Initialize, this);
	GetHost().m_OnCellAdded.Bind(&LightComponent::OnCellAdded, this);
	GetHost().m_OnCellRemoved.Bind(&LightComponent::OnCellRemoved, this);

	GetHost().m_cell->AddRenderer(this);
}

void LightComponent::Initialize()
{
	m_transform = GetHost().GetOrAddBehaviour<Transform>();

	if (m_transform)
	{
		m_transform->m_onPositionChange.Bind(&LightComponent::ChangePosition, this);
		m_transform->m_onRotationChange.Bind(&LightComponent::ChangeRotation, this);
		m_transform->m_OnDestroy.Bind(&LightComponent::InvalidateTransform, this);
	}

	ChangePosition(m_transform->GetPosition());
	ChangeRotation(m_transform->GetRotation());

	GetHost().m_OnAwake.Unbind(&LightComponent::Initialize, this);
}

void LightComponent::PopulateMetadatas()
{
	Behaviour::PopulateMetadatas();

	m_metadatas.SetField("ambient", m_light.m_ambient);
	m_metadatas.SetField("diffuse", m_light.m_diffuse);
	m_metadatas.SetField("specular", m_light.m_specular);
	m_metadatas.SetField("attenuation", m_light.m_attenuation);
	m_metadatas.SetField("isPoint", m_light.m_isPoint);
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

void LightComponent::SubscribeToPipeline(ARenderingPipeline* pipeline)
{
	pipeline->SubscribeToPipeline<ShadowRenderPass>(&m_light);
	pipeline->SubscribeToPipeline<BasicRenderPass>(&m_light);
	pipeline->SubscribeToPipeline<PBRRenderPass>(&m_light);

}

void LightComponent::UnsubscribeToPipeline(ARenderingPipeline* pipeline)
{
	pipeline->UnsubscribeToPipeline<ShadowRenderPass>(&m_light);
	pipeline->UnsubscribeToPipeline<BasicRenderPass>(&m_light);
	pipeline->UnsubscribeToPipeline<PBRRenderPass>(&m_light);
}

void LightComponent::OnCellAdded(Cell* newCell)
{
	newCell->AddRenderer(this);
}

void LightComponent::OnCellRemoved(Cell* newCell)
{
	newCell->RemoveRenderer(this);
}

void LightComponent::InvalidateTransform()
{
	m_transform = nullptr;
}

void LightComponent::SubscribeGuizmo()
{
	if (CameraComponent::m_editorCamera)
		CameraComponent::m_editorCamera->m_pipeline->SubscribeToPipeline<GuizmoRenderPass>(this);
}

void LightComponent::UnsubscribeGuizmo()
{
	if (CameraComponent::m_editorCamera)
		CameraComponent::m_editorCamera->m_pipeline->UnsubscribeToPipeline<GuizmoRenderPass>(this);
}