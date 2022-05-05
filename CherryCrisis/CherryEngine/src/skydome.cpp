#include "pch.hpp"

#include "skydome.hpp"

#include "resource_manager.hpp"

#include "environment_map_renderpass.hpp"
#include "irradiance_map_renderpass.hpp"
#include "prefilter_map_renderpass.hpp"
#include "brdf_renderpass.hpp"
#include "skydome_renderpass.hpp"
#include "render_manager.hpp"
#include "pbr_renderpass.hpp"

#include "spheremap.hpp"
#include "mesh.hpp"
#include "rendering_pipeline_interface.hpp"
#include "viewer.hpp"
#include "cell.hpp"

Skydome::Skydome(Cell* cell)
	: m_cell(cell)
{

}

Skydome::~Skydome()
{
	if (m_mesh)
		m_spheremap->m_OnDeleted.Unbind(&Skydome::RemoveMesh, this);
}

void Skydome::RemoveMesh()
{
	m_mesh = nullptr;
	m_quad = nullptr;

	// Move to function
	m_cell->RemoveRenderer(this);
}

void Skydome::RemoveCubemap()
{
	m_mesh = nullptr;
	m_quad = nullptr;

	// Move to function
	m_cell->RemoveRenderer(this);
}

void Skydome::Load()
{
	m_quad = ResourceManager::GetInstance()->AddResource<Mesh>("CC_normalizedQuad", true, EMeshShape::QUAD, 1.f, 1.f);
	m_quad->m_OnDeleted.Bind(&Skydome::RemoveMesh, this);

	m_mesh = ResourceManager::GetInstance()->AddResource<Mesh>("CC_normalizedCube", true, EMeshShape::CUBE, 0.5f, 0.5f, 0.5f);
	m_mesh->m_OnDeleted.Bind(&Skydome::RemoveMesh, this);

	// TODO: Remove texturePath
	m_spheremap = ResourceManager::GetInstance()->AddResource<Spheremap>("skySphere", true, "Assets/skydome/Arches_3k.hdr");
	m_spheremap->m_OnDeleted.Bind(&Skydome::RemoveCubemap, this);

	// Move to function
	m_cell->AddRenderer(this);
}

void Skydome::ClearData()
{
	m_mesh->ClearData();
	//m_quad->ClearData();
	m_spheremap->ClearData();
}

void Skydome::SubscribeToPipeline(ARenderingPipeline* pipeline)
{
	pipeline->SubscribeToPipeline<EnvironmentMapRenderPass>(this);
	pipeline->SubscribeToPipeline<IrradianceMapRenderPass>(this);
	pipeline->SubscribeToPipeline<PrefilterMapRenderPass>(this);
	pipeline->SubscribeToPipeline<BRDFRenderPass>(this);
	pipeline->SubscribeToPipeline<SkydomeRenderPass>(this);
	pipeline->SubscribeToPipeline<PBRRenderPass>(this);
}

void Skydome::UnsubscribeToPipeline(ARenderingPipeline* pipeline)
{
	pipeline->UnsubscribeToPipeline<EnvironmentMapRenderPass>(this);
	pipeline->UnsubscribeToPipeline<IrradianceMapRenderPass>(this);
	pipeline->UnsubscribeToPipeline<PrefilterMapRenderPass>(this);
	pipeline->UnsubscribeToPipeline<BRDFRenderPass>(this);
	pipeline->UnsubscribeToPipeline<SkydomeRenderPass>(this);
	pipeline->UnsubscribeToPipeline<PBRRenderPass>(this);
}