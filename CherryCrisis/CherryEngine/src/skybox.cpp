#include "pch.hpp"

#include "skybox.hpp"

#include "resource_manager.hpp"

#include "cubemap.hpp"
#include "mesh.hpp"

#include "skybox_renderpass.hpp"
#include "render_manager.hpp"
#include "threadpool.hpp"

#include "rendering_pipeline_interface.hpp"
#include "viewer.hpp"
#include "cell.hpp"

Skybox::Skybox(Cell* cell)
	: m_cell(cell)
{

}

Skybox::~Skybox()
{
	if (m_mesh)
		m_cubemap->m_OnDeleted.Unbind(&Skybox::RemoveMesh, this);

	if (m_mesh)
		m_cubemap->m_OnDeleted.Unbind(&Skybox::RemoveMesh, this);
}

void Skybox::RemoveMesh()
{
	m_mesh = nullptr;

	// Move to function
	m_cell->RemoveRenderer(this);
}

void Skybox::RemoveCubemap()
{
	m_mesh = nullptr;

	// Move to function
	m_cell->RemoveRenderer(this);
}

void Skybox::Load()
{
	m_mesh = ResourceManager::GetInstance()->AddResource<Mesh>("CC_normalizedCube", true, EMeshShape::CUBE, 0.5f, 0.5f, 0.5f);
	m_mesh->m_OnDeleted.Bind(&Skybox::RemoveMesh, this);

	// TODO: Remove this
	const char* textures[6] = { "Assets/Skybox/right.jpg", "Assets/Skybox/left.jpg", "Assets/Skybox/top.jpg", "Assets/Skybox/bottom.jpg", "Assets/Skybox/front.jpg", "Assets/Skybox/back.jpg" };

	m_cubemap = ResourceManager::GetInstance()->AddResource<Cubemap>("skyCubemap", true, textures);
	m_cubemap->m_OnDeleted.Bind(&Skybox::RemoveCubemap, this);

	// Move to function
	m_cell->AddRenderer(this);
}

void Skybox::ClearData()
{
	m_mesh->ClearData();
	m_cubemap->ClearData();
}

void Skybox::SubscribeToPipeline(ARenderingPipeline* pipeline)
{
	pipeline->SubscribeToPipeline<SkyboxRenderPass>(this);
}

void Skybox::UnsubscribeToPipeline(ARenderingPipeline* pipeline)
{
	pipeline->UnsubscribeToPipeline<SkyboxRenderPass>(this);
}
