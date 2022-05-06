#include "pch.hpp"

#include "model_renderer.hpp"

#include "render_manager.hpp"
#include "resource_manager.hpp"

#include "shadow_renderpass.hpp"
#include "basic_renderpass.hpp"
#include "pbr_renderpass.hpp"
#include "entity.hpp"
#include "model.hpp"
#include "picking_renderpass.hpp"
#include "texture.hpp"
#include "material.hpp"

#include "cell.hpp"

int ModelRenderer::count = 1;

ModelRenderer::ModelRenderer()
{
	PopulateMetadatas();
	m_id = count;
	count++;
}

ModelRenderer::ModelRenderer(CCUUID& id) : Behaviour(id)
{
	PopulateMetadatas();
	m_id = count;
	count++;
}

ModelRenderer::~ModelRenderer()
{
//	RemoveModel();
	/*if (m_mesh)
	{
		m_mesh
		GetHost().m_cell->RemoveRenderer(this);
		m_model->m_OnLoaded.Unbind(&ModelRenderer::OnModelLoaded, this);

		m_model->m_OnDeleted.Unbind(&ModelRenderer::RemoveModel, this);
	}

	GetHost().m_cell->RemoveRenderer(this);*/
}

void ModelRenderer::PopulateMetadatas()
{
	Behaviour::PopulateMetadatas();

	m_metadatas.SetField<Object*>("transform", m_transform);
	m_metadatas.SetProperty("meshFile", &m_MeshPath);
	m_metadatas.SetProperty("matFile", &m_MaterialPath);
}

void ModelRenderer::LoadModelFromPath(const char* modelPath)
{
	if (!modelPath)
		return;

	ResourceManager::GetInstance()->AddResourceMultiThreads<ModelBase>(modelPath, true, nullptr);
}

const char* ModelRenderer::GetModelPath()
{
	return m_mesh ? m_mesh->GetFilepath().c_str() : nullptr;
}

void ModelRenderer::SetMeshFromPath(const char* meshPath)
{
	SetMesh(ResourceManager::GetInstance()->AddResourceRef<Mesh>(meshPath));
}

const char* ModelRenderer::GetMeshPath()
{
	return m_mesh ? m_mesh->GetFilepath().c_str() : nullptr;
}

void ModelRenderer::SetMaterialFromPath(const char* materialPath)
{
	SetMaterial(ResourceManager::GetInstance()->AddResourceRef<Material>(materialPath));
}

const char* ModelRenderer::GetMaterialPath()
{
	return m_material ? m_material->GetFilepath().c_str() : nullptr;
}

void ModelRenderer::OnMeshLoaded(std::shared_ptr<Mesh> mesh)
{
	mesh->m_OnLoaded.Unbind(&ModelRenderer::OnMeshLoaded, this);
	SetMesh(mesh);
}

void ModelRenderer::SetMesh(std::shared_ptr<Mesh> newMesh)
{
	if (!newMesh)
	{
		RemoveMesh();
		return;
	}

	EResourceState resourceState = newMesh->GetResourceState();
	if (resourceState != EResourceState::LOADED)
	{
		newMesh->m_OnLoaded.Bind(&ModelRenderer::OnMeshLoaded, this);
		return;
	}

	m_mesh = newMesh;
	m_mesh->m_OnDeleted.Bind(&ModelRenderer::RemoveMesh, this);

	if (m_initialized)
	{
		GetHost().m_cell->RemoveRenderer(this);
		GetHost().m_cell->AddRenderer(this);
	}
}

void ModelRenderer::RemoveMesh() 
{
	if (!m_mesh)
		return;
	
	m_mesh->m_OnDeleted.Unbind(&ModelRenderer::RemoveMesh, this);
	
	GetHost().m_cell->RemoveRenderer(this);
	
	m_mesh = nullptr;
}

void ModelRenderer::OnMaterialReloaded(std::shared_ptr<Material> material)
{
	GetHost().m_cell->RemoveRenderer(this);
	GetHost().m_cell->AddRenderer(this);
}

void ModelRenderer::OnMaterialLoaded(std::shared_ptr<Material> newMat)
{
	newMat->m_OnLoaded.Unbind(&ModelRenderer::OnMaterialLoaded, this);
	SetMaterial(newMat);
}

void ModelRenderer::SetMaterial(std::shared_ptr<Material> newMat)
{
	if (!newMat)
	{
		RemoveMaterial();
		return;
	}

	EResourceState resourceState = newMat->GetResourceState();
	if (resourceState != EResourceState::LOADED)
	{
		newMat->m_OnLoaded.Bind(&ModelRenderer::OnMaterialLoaded, this);
		return;
	}

	m_material = newMat;
	m_material->m_OnDeleted.Bind(&ModelRenderer::RemoveMaterial, this);
	m_material->m_OnReloaded.Bind(&ModelRenderer::OnMaterialReloaded, this);

	if (m_initialized)
	{
		GetHost().m_cell->RemoveRenderer(this);
		GetHost().m_cell->AddRenderer(this);
	}
}

void ModelRenderer::RemoveMaterial()
{
	if (!m_material)
		return;

	m_material->m_OnDeleted.Unbind(&ModelRenderer::RemoveMaterial, this);
	m_material->m_OnReloaded.Unbind(&ModelRenderer::OnMaterialReloaded, this);
	m_material = nullptr;
}

void ModelRenderer::SubscribeToPipeline(ARenderingPipeline* pipeline)
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

void ModelRenderer::UnsubscribeToPipeline(ARenderingPipeline* pipeline)
{

	pipeline->UnsubscribeToPipeline<ShadowRenderPass>(this);
	pipeline->UnsubscribeToPipeline<PBRRenderPass>(this);
	pipeline->UnsubscribeToPipeline<BasicRenderPass>(this);
	pipeline->UnsubscribeToPipeline<PickingRenderPass>(this);
}

void ModelRenderer::Initialize() 
{
	m_initialized = true;

	if (m_mesh)
		GetHost().m_cell->AddRenderer(this);

	GetHost().m_OnAwake.Unbind(&ModelRenderer::Initialize, this);
}

void ModelRenderer::BindToSignals() 
{
	GetHost().m_OnAwake.Bind(&ModelRenderer::Initialize, this);
	GetHost().m_OnCellAdded.Bind(&ModelRenderer::OnCellAdded, this);
	GetHost().m_OnCellRemoved.Bind(&ModelRenderer::OnCellRemoved, this);
}

void ModelRenderer::OnCellAdded(Cell* newCell)
{
	GetHost().m_OnCellAdded.Unbind(&ModelRenderer::OnCellAdded, this);

	if (m_mesh)
		newCell->AddRenderer(this);
}

void ModelRenderer::OnCellRemoved(Cell* newCell)
{
	GetHost().m_OnCellRemoved.Unbind(&ModelRenderer::OnCellRemoved, this);

	if (m_mesh)
		newCell->RemoveRenderer(this);
}
