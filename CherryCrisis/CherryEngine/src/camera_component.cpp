#include "pch.hpp"

#include "camera_component.hpp"

#include "cell.hpp"

#include "transform.hpp"

#include "basic_renderpass.hpp"
#include "skybox_renderpass.hpp"
#include "guizmo_renderpass.hpp"
#include "render_manager.hpp"
#include "camera_component.hpp"

#include "mixed_rendering_pipeline.hpp"
#include "entity.hpp"

unsigned int	 CameraComponent::m_count	    = 0;
CameraComponent* CameraComponent::m_mainCamera      = nullptr;
Camera*          CameraComponent::m_editorCamera	= nullptr;

CameraComponent::CameraComponent()
{
	if (m_count == 0) SetAsMain();
	m_count++;
	m_camera.m_pipeline = std::make_unique<MixedPipeline>();
	PopulateMetadatas();
	if (m_editorCamera)
		SubscribeToPipeline(m_editorCamera->m_pipeline.get());
}

CameraComponent::CameraComponent(CCUUID& id) : Behaviour(id)
{
	if (m_count == 0) SetAsMain();
	m_count++;
	m_camera.m_pipeline = std::make_unique<MixedPipeline>();
	PopulateMetadatas();
	if (m_editorCamera)
		SubscribeToPipeline(m_editorCamera->m_pipeline.get());
}

CameraComponent::~CameraComponent()
{
	if (m_editorCamera)
		UnsubscribeToPipeline(m_editorCamera->m_pipeline.get());

	if (m_mainCamera == this) m_mainCamera = nullptr;

	m_count--;
	GetHost().m_cell->RemoveViewer(&m_camera);

	if (m_transform)
	{
		m_transform->m_onPositionChange.Unbind(&CameraComponent::ChangePosition, this);
		m_transform->m_onRotationChange.Unbind(&CameraComponent::ChangeRotation, this);
		m_transform->m_OnDestroy.Unbind(&CameraComponent::InvalidateTransform, this);
	}
}

void CameraComponent::PopulateMetadatas()
{
	Behaviour::PopulateMetadatas();

	m_metadatas.SetField("aspect", *const_cast<float*>(m_camera.GetAspectPtr()));
	m_metadatas.SetField("near", *const_cast<float*>(m_camera.GetNearPtr()));
	m_metadatas.SetField("far", *const_cast<float*>(m_camera.GetFarPtr()));
	m_metadatas.SetField("fovY", *const_cast<float*>(m_camera.GetFovYPtr()));
	m_metadatas.SetField<Object*>("transform", m_transform);
}

void CameraComponent::BindToSignals()
{
	GetHost().m_OnAwake.Bind(&CameraComponent::Initialize, this);

	GetHost().m_cell->AddViewer(&m_camera);
}

void CameraComponent::Initialize() 
{
	m_transform = GetHost().GetOrAddBehaviour<Transform>();

	if (m_transform)
	{
		m_transform->m_onPositionChange.Bind(&CameraComponent::ChangePosition, this);
		m_transform->m_onRotationChange.Bind(&CameraComponent::ChangeRotation, this);
		m_transform->m_OnDestroy.Bind(&CameraComponent::InvalidateTransform, this);
	}

	GetHost().m_OnAwake.Unbind(&CameraComponent::Initialize, this);

	m_camera.m_framebuffer->Init();

	ChangePosition(m_transform->GetPosition());
	ChangeRotation(m_transform->GetRotation());
}

void CameraComponent::ChangePosition(const CCMaths::Vector3& position)
{
	m_camera.SetPosition(m_transform->GetGlobalPosition());
}

void CameraComponent::ChangeRotation(const CCMaths::Vector3& rotation)
{
	m_camera.SetRotation(m_transform->GetGlobalRotation());
}

CameraComponent* CameraComponent::GetMainCamera()
{
	return m_mainCamera;
}

void CameraComponent::SetAsMain() 
{
	m_mainCamera = this;
}

void CameraComponent::InvalidateTransform()
{
	m_transform = nullptr;
}

void CameraComponent::SubscribeToPipeline(ARenderingPipeline* pipeline)
{
	m_editorCamera->m_pipeline->SubscribeToPipeline<GuizmoRenderPass>(this);
}

void CameraComponent::UnsubscribeToPipeline(ARenderingPipeline* pipeline)
{
	m_editorCamera->m_pipeline->UnsubscribeToPipeline<GuizmoRenderPass>(this);
}

