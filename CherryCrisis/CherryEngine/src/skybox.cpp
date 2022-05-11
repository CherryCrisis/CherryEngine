#include "pch.hpp"

#include "skybox.hpp"

#include "resource_manager.hpp"

#include "cubemap.hpp"

#include "skybox_renderpass.hpp"

#include "rendering_pipeline_interface.hpp"
#include "viewer.hpp"
#include "cell.hpp"

Skybox::Skybox(Cell* cell)
	: SkyRenderer(cell)
{

}

Skybox::~Skybox()
{
	RemoveMap();
}

void Skybox::RemoveMap()
{
	if (m_cubemap)
	{
		m_cubemap->m_OnDeleted.Unbind(&Skybox::RemoveMap, this);
		m_cubemap = nullptr;

		// Move to function
		m_cell->RemoveRenderer(this);
	}
}

void Skybox::Load()
{
	m_cube = ResourceManager::GetInstance()->AddResource<Mesh>("CC_normalizedCube", true, EMeshShape::CUBE, 0.5f, 0.5f, 0.5f);
	m_cube->m_OnDeleted.Bind(&SkyRenderer::RemoveCube, static_cast<SkyRenderer*>(this));

	// Move to function
	m_cell->AddRenderer(this);
}

void Skybox::ClearData()
{
	m_cube->ClearData();
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
