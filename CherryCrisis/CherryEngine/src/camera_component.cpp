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
}

CameraComponent::~CameraComponent()
{
	auto RM = RenderManager::GetInstance();

	RM->RemoveFromPipeline<BasicRenderPass>(this);
	RM->RemoveFromPipeline<SkyboxRenderPass>(this);
}