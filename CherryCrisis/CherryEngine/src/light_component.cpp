#include <pch.hpp>

#include "light_component.hpp"

#include "render_manager.hpp"

#include "camera_component.hpp"
#include "cell.hpp"
#include "entity.hpp"
#include "maths.hpp"
#include "transform.hpp"
#include "basic_renderpass.hpp"
#include "guizmo_renderpass.hpp"
#include "pbr_renderpass.hpp"
#include "shadow_renderpass.hpp"


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

	m_metadatas.SetProperty("light type", &lightType);
	m_metadatas.SetProperty("ambient", &ambient);
	m_metadatas.SetProperty("diffuse", &diffuse);
	m_metadatas.SetProperty("specular", &specular);
	m_metadatas.SetProperty("attenuation", &attenuation);
	m_metadatas.SetProperty("outer cutoff", &cutoff);
	m_metadatas.SetProperty("cutoff", &outerCutoff);
}

void LightComponent::ChangePosition(const CCMaths::Vector3& position)
{
	m_light.m_position = position;

	m_light.m_lookAtMatrix = CCMaths::Matrix4::LookAt(m_light.m_position);

	// TODO: Add real ortho values
	m_light.m_lightSpace = CCMaths::Matrix4::Orthographic(-10.f, 10.f, -10.f, 10.f, -50.f, 20.f) * m_light.m_lookAtMatrix;

	m_light.m_OnParamsChanged.Invoke();
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

void LightComponent::SetPosition(const CCMaths::Vector3& newPosition)
{
	m_light.m_position = newPosition;
	m_light.m_OnParamsChanged.Invoke();
}

void LightComponent::SetRotation(const CCMaths::Vector3& newDirection)
{
	m_light.m_direction = newDirection;
	m_light.m_OnParamsChanged.Invoke();
}

void LightComponent::SetAmbient(const CCMaths::Vector3& newAmbient)
{
	m_light.m_ambient = newAmbient;
	m_light.m_OnParamsChanged.Invoke();
}

void LightComponent::SetDiffuse(const CCMaths::Vector3& newDiffuse)
{
	m_light.m_diffuse = newDiffuse;
	m_light.m_OnParamsChanged.Invoke();
}

void LightComponent::SetSpecular(const CCMaths::Vector3& newSpecular)
{
	m_light.m_specular = newSpecular;
	m_light.m_OnParamsChanged.Invoke();
}

void LightComponent::SetAttenuation(const CCMaths::Vector3& newAttenuation)
{
	m_light.m_attenuation = newAttenuation;
	m_light.m_OnParamsChanged.Invoke();
}

void LightComponent::SetCutoff(float newCutoff)
{
	m_cutoff = newCutoff;
	m_light.m_cutoff = cosf(newCutoff);
	m_light.m_OnParamsChanged.Invoke();
}

void LightComponent::SetOuterCutoff(float newOuterCutoff)
{
	m_outerCutoff = newOuterCutoff;

	m_light.m_outerCutoff = cosf(newOuterCutoff);
	m_light.m_OnParamsChanged.Invoke();
}

void LightComponent::SetLightType(ELightType newType)
{
	m_light.m_type = newType;
	m_light.m_OnParamsChanged.Invoke();
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