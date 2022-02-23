#include "skybox.hpp"

#include "resourceManager.hpp"

#include "cubemap.hpp"
#include "mesh.hpp"

#include "skybox_pipeline.hpp"
#include "render_manager.hpp"

Skybox::Skybox()
{
	m_mesh = ResourceManager::GetInstance()->AddResource<Mesh>("NormalizedCube", true, EMeshShape::CUBE, 0.5f, 0.5f, 0.5f);

	RenderManager::instance()->GenerateFromPipeline<SkyboxSubPipeline>(this);
}