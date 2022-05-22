#include <pch.hpp>

#include "model_renderer.hpp"

#include <string>

#include "render_manager.hpp"
#include "resource_manager.hpp"

#include "cell.hpp"
#include "entity.hpp"
#include "material.hpp"
#include "model.hpp"
#include "texture.hpp"
#include "basic_renderpass.hpp"
#include "picking_renderpass.hpp"
#include "pbr_renderpass.hpp"
#include "shadow_renderpass.hpp"


int MeshRenderer::count = 1;

MeshRenderer::MeshRenderer()
{
	PopulateMetadatas();
	m_id = count;
	count++;
}

MeshRenderer::MeshRenderer(CCUUID& id) : Behaviour(id)
{
	PopulateMetadatas();
	m_id = count;
	count++;
}

void MeshRenderer::PopulateMetadatas()
{
	Behaviour::PopulateMetadatas();

	m_metadatas.SetField<Behaviour*>("transform", m_transform);
	m_metadatas.SetProperty("matFile", &m_MaterialPath);
}

void MeshRenderer::SetMaterialFromPath(std::string materialPath)
{
	if (!materialPath.empty())
		SetMaterial(ResourceManager::GetInstance()->AddResourceRef<Material>(materialPath.c_str()));
}

std::string MeshRenderer::GetMaterialPath()
{
	return m_material ? m_material->GetFilepath() : "";
}

void MeshRenderer::OnMeshLoaded(std::shared_ptr<Mesh> mesh)
{
	mesh->m_OnLoaded.Unbind(&MeshRenderer::OnMeshLoaded, this);
	SetMesh(mesh);
}

void MeshRenderer::SetMesh(std::shared_ptr<Mesh> newMesh)
{
	if (!newMesh)
	{
		RemoveMesh();
		return;
	}

	EResourceState resourceState = newMesh->GetResourceState();
	if (resourceState != EResourceState::LOADED)
	{
		newMesh->m_OnLoaded.Bind(&MeshRenderer::OnMeshLoaded, this);
		return;
	}

	m_mesh = newMesh;
	m_mesh->m_OnDeleted.Bind(&MeshRenderer::RemoveMesh, this);

	if (m_initialized)
	{
		GetHost().m_cell->RemoveRenderer(this);
		GetHost().m_cell->AddRenderer(this);
	}
}

void MeshRenderer::RemoveMesh()
{
	if (!m_mesh)
		return;

	m_mesh->m_OnDeleted.Unbind(&MeshRenderer::RemoveMesh, this);

	GetHost().m_cell->RemoveRenderer(this);

	m_mesh = nullptr;
}

void MeshRenderer::OnMaterialReloaded(std::shared_ptr<Material> material)
{
	GetHost().m_cell->RemoveRenderer(this);
	GetHost().m_cell->AddRenderer(this);
}

void MeshRenderer::OnMaterialLoaded(std::shared_ptr<Material> newMat)
{
	newMat->m_OnLoaded.Unbind(&MeshRenderer::OnMaterialLoaded, this);
	SetMaterial(newMat);
}

void MeshRenderer::SetMaterial(std::shared_ptr<Material> newMat)
{
	if (!newMat)
	{
		RemoveMaterial();
		return;
	}

	EResourceState resourceState = newMat->GetResourceState();
	if (resourceState != EResourceState::LOADED)
	{
		newMat->m_OnLoaded.Bind(&MeshRenderer::OnMaterialLoaded, this);
		return;
	}

	m_material = newMat;
	m_material->m_OnDeleted.Bind(&MeshRenderer::RemoveMaterial, this);
	m_material->m_OnReloaded.Bind(&MeshRenderer::OnMaterialReloaded, this);

	if (m_initialized)
	{
		GetHost().m_cell->RemoveRenderer(this);
		GetHost().m_cell->AddRenderer(this);
	}
}

void MeshRenderer::RemoveMaterial()
{
	if (!m_material)
		return;

	m_material->m_OnDeleted.Unbind(&MeshRenderer::RemoveMaterial, this);
	m_material->m_OnReloaded.Unbind(&MeshRenderer::OnMaterialReloaded, this);
	m_material = nullptr;
}

void MeshRenderer::SubscribeToPipeline(ARenderingPipeline* pipeline)
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

void MeshRenderer::UnsubscribeToPipeline(ARenderingPipeline* pipeline)
{
	pipeline->UnsubscribeToPipeline<ShadowRenderPass>(this);
	pipeline->UnsubscribeToPipeline<PBRRenderPass>(this);
	pipeline->UnsubscribeToPipeline<BasicRenderPass>(this);
	pipeline->UnsubscribeToPipeline<PickingRenderPass>(this);
}