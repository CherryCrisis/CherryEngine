#include "pch.hpp"

#include "camera_component.hpp"

#include "basic_renderpass.hpp"
#include "skybox_renderpass.hpp"
#include "render_manager.hpp"

CameraComponent::CameraComponent(Entity& owner)
	: Behaviour(owner)
{
	auto RM = RenderManager::GetInstance();

	RM->GenerateFromPipeline<BasicRenderPass>(this);
	RM->GenerateFromPipeline<SkyboxRenderPass>(this);

	PopulateMetadatas();
}

CameraComponent::CameraComponent()
{
	auto RM = RenderManager::GetInstance();

	RM->GenerateFromPipeline<BasicRenderPass>(this);
	RM->GenerateFromPipeline<SkyboxRenderPass>(this);

	PopulateMetadatas();
}

void CameraComponent::PopulateMetadatas()
{
	m_metadatas.SetField<float>("aspect", m_camera.aspect);
	m_metadatas.SetField<float>("near", m_camera.near);
	m_metadatas.SetField<float>("far", m_camera.far);
	m_metadatas.SetField<float>("fovY", m_camera.fovY);
	m_metadatas.SetField<Behaviour*>("transform", m_transform);
}

CameraComponent::~CameraComponent()
{
	auto RM = RenderManager::GetInstance();

	RM->RemoveFromPipeline<BasicRenderPass>(this);
	RM->RemoveFromPipeline<SkyboxRenderPass>(this);
}