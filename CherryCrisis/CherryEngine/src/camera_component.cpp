#include "camera_component.hpp"

#include "basic_subpipeline.hpp"
#include "skybox_pipeline.hpp"
#include "render_manager.hpp"

CameraComponent::CameraComponent()
{
	RenderManager::instance()->GenerateFromPipeline<BasicSubPipeline>(this);
	RenderManager::instance()->GenerateFromPipeline<SkyboxSubPipeline>(this);
}

CameraComponent::~CameraComponent()
{
	RenderManager::instance()->RemoveFromPipeline<BasicSubPipeline>(this);
	RenderManager::instance()->RemoveFromPipeline<SkyboxSubPipeline>(this);
}