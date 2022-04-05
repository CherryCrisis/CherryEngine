#include "pch.hpp"

#include "camera_component.hpp"

#include "basic_renderpass.hpp"
#include "skybox_renderpass.hpp"
#include "render_manager.hpp"

CameraComponent::CameraComponent(Entity& owner)
	: Behaviour(owner)
{
	m_transform = owner.GetBehaviour<Transform>();

	auto RM = RenderManager::GetInstance();

	RM->GenerateFromPipeline<BasicRenderPass>(this);
	RM->GenerateFromPipeline<SkyboxRenderPass>(this);

	m_transform->m_onPositionChange.Bind(&CameraComponent::ChangePosition, this);
	m_transform->m_onRotationChange.Bind(&CameraComponent::ChangeRotation, this);

	PopulateMetadatas();
}

CameraComponent::CameraComponent()
{
	auto RM = RenderManager::GetInstance();

	RM->GenerateFromPipeline<BasicRenderPass>(this);
	RM->GenerateFromPipeline<SkyboxRenderPass>(this);

	PopulateMetadatas();
}

CameraComponent::~CameraComponent()
{
	auto RM = RenderManager::GetInstance();

	RM->RemoveFromPipeline<BasicRenderPass>(this);
	RM->RemoveFromPipeline<SkyboxRenderPass>(this);

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

void CameraComponent::ChangePosition(const Vector3& position)
{
	m_camera.position = position;
}

void CameraComponent::ChangeRotation(const Vector3& rotation)
{
	m_camera.rotation = rotation;
}