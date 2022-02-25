#include "pch.hpp"

#include "camera_component.hpp"

#include "basic_renderpass.hpp"
#include "skybox_renderpass.hpp"
#include "render_manager.hpp"

CameraComponent::CameraComponent()
{
	RenderManager::instance()->GenerateFromPipeline<BasicRenderPass>(this);
	RenderManager::instance()->GenerateFromPipeline<SkyboxRenderPass>(this);
}

CameraComponent::~CameraComponent()
{
	RenderManager::instance()->RemoveFromPipeline<BasicRenderPass>(this);
	RenderManager::instance()->RemoveFromPipeline<SkyboxRenderPass>(this);
}