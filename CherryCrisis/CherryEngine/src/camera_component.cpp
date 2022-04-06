#include "pch.hpp"

#include "camera_component.hpp"

#include "basic_renderpass.hpp"
#include "skybox_renderpass.hpp"
#include "render_manager.hpp"

CameraComponent::CameraComponent()
{
	auto RM = RenderManager::GetInstance();

	RM->GenerateFromPipeline<BasicRenderPass>(&m_camera);
	RM->GenerateFromPipeline<SkyboxRenderPass>(&m_camera);

	PopulateMetadatas();
}

CameraComponent::~CameraComponent()
{
	auto RM = RenderManager::GetInstance();

	RM->RemoveFromPipeline<BasicRenderPass>(&m_camera);
	RM->RemoveFromPipeline<SkyboxRenderPass>(&m_camera);

	// TODO: Unbind not in destructor
	// m_transform->m_onPositionChange.Unbind(&CameraComponent::ChangePosition, this);
	// m_transform->m_onRotationChange.Unbind(&CameraComponent::ChangeRotation, this);
}

void CameraComponent::PopulateMetadatas()
{
	m_metadatas.SetField<float>("aspect", m_camera.aspect);
	m_metadatas.SetField<float>("near", m_camera.near);
	m_metadatas.SetField<float>("far", m_camera.far);
	m_metadatas.SetField<float>("fovY", m_camera.fovY);
	m_metadatas.SetField<Behaviour*>("transform", m_transform);
}

void CameraComponent::BindToSignals()
{
	GetHost().m_OnAwake.Bind(&CameraComponent::Initialize, this);
}

void CameraComponent::Initialize() 
{
	m_transform = GetHost().GetBehaviour<Transform>();

	m_transform->m_onPositionChange.Bind(&CameraComponent::ChangePosition, this);
	m_transform->m_onRotationChange.Bind(&CameraComponent::ChangeRotation, this);

	GetHost().m_OnAwake.Unbind(&CameraComponent::Initialize, this);
}

void CameraComponent::ChangePosition(const Vector3& position)
{
	m_camera.position = position;
}

void CameraComponent::ChangeRotation(const Vector3& rotation)
{
	m_camera.rotation = rotation;
}