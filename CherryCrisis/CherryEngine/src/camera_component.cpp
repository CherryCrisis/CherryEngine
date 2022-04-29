#include "pch.hpp"

#include "camera_component.hpp"

#include "cell.hpp"

#include "transform.hpp"

#include "basic_renderpass.hpp"
#include "skybox_renderpass.hpp"
#include "render_manager.hpp"

#include "basic_rendering_pipeline.hpp"

CameraComponent::CameraComponent()
{
	m_camera.m_pipeline = std::make_unique<BasicRPipeline>();
	PopulateMetadatas();
}

CameraComponent::CameraComponent(CCUUID& id) : Behaviour(id)
{
	m_camera.m_pipeline = std::make_unique<BasicRPipeline>();
	PopulateMetadatas();
}

CameraComponent::~CameraComponent()
{
	GetHost().m_cell->RemoveViewer(&m_camera);
}

void CameraComponent::PopulateMetadatas()
{
	Behaviour::PopulateMetadatas();

	m_metadatas.SetField<float>("aspect", m_camera.aspect);
	m_metadatas.SetField<float>("near", m_camera.near);
	m_metadatas.SetField<float>("far", m_camera.far);
	m_metadatas.SetField<float>("fovY", m_camera.fovY);
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

	m_transform->m_onPositionChange.Bind(&CameraComponent::ChangePosition, this);
	m_transform->m_onRotationChange.Bind(&CameraComponent::ChangeRotation, this);

	GetHost().m_OnAwake.Unbind(&CameraComponent::Initialize, this);

	ChangePosition(m_transform->GetPosition());
	ChangeRotation(m_transform->GetRotation());
}

void CameraComponent::ChangePosition(const CCMaths::Vector3& position)
{
	m_camera.lastPosition = m_camera.position;
	m_camera.position = position;

	UpdateCameraModel();
}

void CameraComponent::ChangeRotation(const CCMaths::Vector3& rotation)
{
	m_camera.rotation = rotation;

	UpdateCameraModel();
}

void CameraComponent::UpdateCameraModel()
{
	m_camera.m_viewMatrix = Matrix4::RotateZXY(-m_camera.rotation) * Matrix4::Translate(-m_camera.position);
}