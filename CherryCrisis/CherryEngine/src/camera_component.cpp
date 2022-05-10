#include "pch.hpp"

#include "camera_component.hpp"

#include "cell.hpp"

#include "transform.hpp"

#include "basic_renderpass.hpp"
#include "skybox_renderpass.hpp"
#include "render_manager.hpp"

#include "mixed_rendering_pipeline.hpp"

unsigned int	 CameraComponent::m_count	    = 0;
CameraComponent* CameraComponent::m_mainCamera	= nullptr;

CameraComponent::CameraComponent()
{
	if (m_count == 0) SetAsMain();
	m_count++;
	m_camera.m_pipeline = std::make_unique<MixedPipeline>();
	PopulateMetadatas();
}

CameraComponent::CameraComponent(CCUUID& id) : Behaviour(id)
{
	if (m_count == 0) SetAsMain();
	m_count++;
	m_camera.m_pipeline = std::make_unique<MixedPipeline>();
	PopulateMetadatas();
}

CameraComponent::~CameraComponent()
{
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

	m_metadatas.SetField("aspect", m_camera.aspect);
	m_metadatas.SetField("near", m_camera.near);
	m_metadatas.SetField("far", m_camera.far);
	m_metadatas.SetField("fovY", m_camera.fovY);
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

	ChangePosition(m_transform->GetPosition());
	ChangeRotation(m_transform->GetRotation());
}

void CameraComponent::ChangePosition(const CCMaths::Vector3& position)
{
	m_camera.lastPosition = m_camera.m_position;
	m_camera.m_position = m_transform->GetGlobalPosition();

	UpdateCameraModel();
}

void CameraComponent::ChangeRotation(const CCMaths::Vector3& rotation)
{
	m_camera.rotation = m_transform->GetGlobalRotation();

	UpdateCameraModel();
}

void CameraComponent::UpdateCameraModel()
{
	m_camera.m_viewMatrix = Matrix4::RotateXYZ(-m_camera.rotation) * Matrix4::Translate(-m_camera.m_position);
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

