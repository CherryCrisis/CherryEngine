#include "camera_component.hpp"

#include "basic_subpipeline.hpp"
#include "render_manager.hpp"

CameraComponent::CameraComponent()
{
	RenderManager::instance()->GenerateFromPipeline<BasicSubPipeline>(this);
}

CameraComponent::~CameraComponent()
{
	RenderManager::instance()->RemoveFromPipeline<BasicSubPipeline>(this);
}