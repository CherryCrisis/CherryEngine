#include "pch.hpp"

#include "skybox.hpp"

#include "resource_manager.hpp"

#include "cubemap.hpp"
#include "mesh.hpp"

#include "skybox_renderpass.hpp"
#include "render_manager.hpp"

Skybox::Skybox()
{
	m_mesh = ResourceManager::GetInstance()->AddResource<Mesh>("CC_normalizedCube", true, EMeshShape::CUBE, 0.5f, 0.5f, 0.5f);

	// TODO: Remove this
	const char* textures[6] = { "Assets/right.jpg", "Assets/left.jpg", "Assets/top.jpg", "Assets/bottom.jpg", "Assets/front.jpg", "Assets/back.jpg" };

	m_cubemap = ResourceManager::GetInstance()->AddResource<Cubemap>("skyCubemap", true, textures);

	RenderManager::instance()->GenerateFromPipeline<SkyboxRenderPass>(this);
}