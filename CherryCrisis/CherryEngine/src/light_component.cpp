#include "light_component.hpp"

#include "basic_subpipeline.hpp"
#include "render_manager.hpp"

LightComponent::LightComponent()
{
	RenderManager::instance()->GenerateFromPipeline<BasicSubPipeline>(&m_light);
}

LightComponent::~LightComponent()
{
	RenderManager::instance()->RemoveFromPipeline<BasicSubPipeline>(&m_light);
}
