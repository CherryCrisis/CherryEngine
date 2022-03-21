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

void CameraComponent::PopulateMetadatas()
{
	m_metadatas.m_fields.push_back({ "aspect", &m_camera.aspect });
	m_metadatas.m_fields.push_back({ "near",  &m_camera.near });
	m_metadatas.m_fields.push_back({ "far",  &m_camera.far });
	m_metadatas.m_fields.push_back({ "fovY",  &m_camera.fovY });
}

CameraComponent::~CameraComponent()
{
	auto RM = RenderManager::GetInstance();

	RM->RemoveFromPipeline<BasicRenderPass>(this);
	RM->RemoveFromPipeline<SkyboxRenderPass>(this);
}