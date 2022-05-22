#include <pch.hpp>

#include "shape_renderer.hpp"

#include <string>

#include "render_manager.hpp"
#include "resource_manager.hpp"

#include "cell.hpp"
#include "entity.hpp"
#include "material.hpp"
#include "model.hpp"
#include "texture.hpp"
#include "transform.hpp"
#include "basic_renderpass.hpp"
#include "pbr_renderpass.hpp"
#include "picking_renderpass.hpp"
#include "shadow_renderpass.hpp"


// TODO: change with something working
int ShapeRenderer::count = 100;

ShapeRenderer::ShapeRenderer()
{
	PopulateMetadatas();
	m_id = count;
	count++;

	m_material = nullptr;
}

ShapeRenderer::ShapeRenderer(CCUUID& id) : Behaviour(id)
{
	PopulateMetadatas();
	m_id = count;
	count++;

	m_material = nullptr;
}

ShapeRenderer::~ShapeRenderer()
{
	RemoveMesh();
	RemoveMaterial();

	GetHost().m_cell->RemoveRenderer(this);

	GetHost().m_OnCellAdded.Unbind(&ShapeRenderer::OnCellAdded, this);
	GetHost().m_OnCellRemoved.Unbind(&ShapeRenderer::OnCellRemoved, this);
}

void ShapeRenderer::PopulateMetadatas()
{
	Behaviour::PopulateMetadatas();

	m_metadatas.SetField<Behaviour*>("transform", m_transform);
	m_metadatas.SetProperty("meshFile", &m_MeshPath);
	m_metadatas.SetProperty("matFile", &m_MaterialPath);
}

void ShapeRenderer::SetMeshFromPath(std::string meshPath)
{
	if (!meshPath.empty())
		SetMesh(ResourceManager::GetInstance()->AddResourceRef<Mesh>(meshPath.c_str()));
}

std::string ShapeRenderer::GetMeshPath()
{
	return m_mesh ? m_mesh->GetFilepath() : "";
}

void ShapeRenderer::SetMaterialFromPath(std::string materialPath)
{
	if (!materialPath.empty())
		SetMaterial(ResourceManager::GetInstance()->AddResourceRef<Material>(materialPath.c_str()));
}

std::string ShapeRenderer::GetMaterialPath()
{
	return m_material ? m_material->GetFilepath() : "";
}

void ShapeRenderer::OnMeshLoaded(std::shared_ptr<Mesh> mesh)
{
	mesh->m_OnLoaded.Unbind(&ShapeRenderer::OnMeshLoaded, this);
	SetMesh(mesh);
}

void ShapeRenderer::SetMesh(std::shared_ptr<Mesh> newMesh)
{
	if (!newMesh)
	{
		RemoveMesh();
		return;
	}

	EResourceState resourceState = newMesh->GetResourceState();
	if (resourceState != EResourceState::LOADED)
	{
		newMesh->m_OnLoaded.Bind(&ShapeRenderer::OnMeshLoaded, this);
		return;
	}

	m_mesh = newMesh;
	m_mesh->m_OnDeleted.Bind(&ShapeRenderer::RemoveMesh, this);

	if (m_initialized)
	{
		GetHost().m_cell->RemoveRenderer(this);
		GetHost().m_cell->AddRenderer(this);
	}
}

void ShapeRenderer::RemoveMesh()
{
	if (!m_mesh)
		return;

	m_mesh->m_OnDeleted.Unbind(&ShapeRenderer::RemoveMesh, this);

	GetHost().m_cell->RemoveRenderer(this);

	m_mesh = nullptr;
}

void ShapeRenderer::OnMaterialReloaded(std::shared_ptr<Material> material)
{
	GetHost().m_cell->RemoveRenderer(this);
	GetHost().m_cell->AddRenderer(this);
}

void ShapeRenderer::OnMaterialLoaded(std::shared_ptr<Material> newMat)
{
	newMat->m_OnLoaded.Unbind(&ShapeRenderer::OnMaterialLoaded, this);
	SetMaterial(newMat);
}

void ShapeRenderer::SetMaterial(std::shared_ptr<Material> newMat)
{
	if (!newMat)
	{
		RemoveMaterial();
		return;
	}

	EResourceState resourceState = newMat->GetResourceState();
	if (resourceState != EResourceState::LOADED)
	{
		newMat->m_OnLoaded.Bind(&ShapeRenderer::OnMaterialLoaded, this);
		return;
	}

	m_material = newMat;
	m_material->m_OnDeleted.Bind(&ShapeRenderer::RemoveMaterial, this);
	m_material->m_OnReloaded.Bind(&ShapeRenderer::OnMaterialReloaded, this);

	if (m_initialized)
	{
		GetHost().m_cell->RemoveRenderer(this);
		GetHost().m_cell->AddRenderer(this);
	}
}

void ShapeRenderer::RemoveMaterial()
{
	if (!m_material)
		return;

	m_material->m_OnDeleted.Unbind(&ShapeRenderer::RemoveMaterial, this);
	m_material->m_OnReloaded.Unbind(&ShapeRenderer::OnMaterialReloaded, this);
	m_material = nullptr;
}

void ShapeRenderer::SubscribeToPipeline(ARenderingPipeline* pipeline)
{
	if (!m_mesh)
		return;

	pipeline->SubscribeToPipeline<ShadowRenderPass>(this);
	pipeline->SubscribeToPipeline<PickingRenderPass>(this);

	if (m_material)
	{
		switch (m_material->m_pipelineType)
		{
		default:
		case EPipelineType::LIT:
			pipeline->SubscribeToPipeline<BasicRenderPass>(this);
			break;

		case EPipelineType::PBR:
			pipeline->SubscribeToPipeline<PBRRenderPass>(this);
			break;
		}
	}
	else
		pipeline->SubscribeToPipeline<BasicRenderPass>(this);

}

void ShapeRenderer::UnsubscribeToPipeline(ARenderingPipeline* pipeline)
{
	pipeline->UnsubscribeToPipeline<ShadowRenderPass>(this);
	pipeline->UnsubscribeToPipeline<PBRRenderPass>(this);
	pipeline->UnsubscribeToPipeline<BasicRenderPass>(this);
	pipeline->UnsubscribeToPipeline<PickingRenderPass>(this);
}

void ShapeRenderer::Initialize()
{
	m_initialized = true;

	if (m_mesh)
		GetHost().m_cell->AddRenderer(this);

	GetHost().m_OnAwake.Unbind(&ShapeRenderer::Initialize, this);
}

void ShapeRenderer::BindToSignals()
{
	GetHost().m_OnAwake.Bind(&ShapeRenderer::Initialize, this);
	GetHost().m_OnCellAdded.Bind(&ShapeRenderer::OnCellAdded, this);
	GetHost().m_OnCellRemoved.Bind(&ShapeRenderer::OnCellRemoved, this);
}

void ShapeRenderer::OnCellAdded(Cell* newCell)
{
	if (m_mesh)
		newCell->AddRenderer(this);
}

void ShapeRenderer::OnCellRemoved(Cell* newCell)
{
	if (m_mesh)
		newCell->RemoveRenderer(this);
}
