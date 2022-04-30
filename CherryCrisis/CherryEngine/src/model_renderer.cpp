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
	if (m_model)
		GetHost().m_cell->RemoveRenderer(this);


	GetHost().m_cell->RemoveRenderer(this);
}

void ModelRenderer::PopulateMetadatas()
{
	Behaviour::PopulateMetadatas();

	m_metadatas.SetField<Object*>("transform", m_transform);
	m_metadatas.SetField<std::string>("file", model_path);
}

void ModelRenderer::SetModel(std::shared_ptr<Model> newModel)
{
	if (!newModel)
	{
		RemoveModel();
		return;
	}

	m_model = newModel;

	model_path = m_model->GetFilepath();
	
	m_model->m_OnDeleted.Bind(&ModelRenderer::RemoveModel, this);
	if (m_model->m_material) 
	{
		SetMaterial(m_model->m_material.get());
	}
	else 
	{
		m_model->m_onMaterialSet.Bind(&ModelRenderer::SetMaterial, this);
	}
	// Move to function
	if (m_initialized)
		GetHost().m_cell->AddRenderer(this);
}

void ModelRenderer::RemoveModel() 
{
	if (!m_model)
		return;

	m_model->m_OnDeleted.Unbind(&ModelRenderer::RemoveModel, this);

	GetHost().m_cell->RemoveRenderer(this);

	m_model = nullptr;
}

void ModelRenderer::SubscribeToPipeline(ARenderingPipeline* pipeline)
{
	if (!m_model)
		return;

	if (Material* material = m_model->m_material.get())
	{
		pipeline->SubscribeToPipeline<ShadowRenderPass>(this);

		switch (material->m_pipelineType)
		{
		case EPipelineType::LIT:
			pipeline->SubscribeToPipeline<BasicRenderPass>(this);
			break;
		case EPipelineType::PBR:
			pipeline->SubscribeToPipeline<PBRRenderPass>(this);
			break;
		default:
			break;
		}
		
		pipeline->SubscribeToPipeline<PickingRenderPass>(this);
	}

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

	if (m_model)
		// Move to function
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
	if (m_model)
		newCell->AddRenderer(this);
}

void ModelRenderer::OnCellRemoved(Cell* newCell)
{
	if (m_model)
		newCell->RemoveRenderer(this);
}

void ModelRenderer::SetMaterial(Material* newMat)
{
	newMat->m_onTextureSet.Bind(&ModelRenderer::ReloadTexture, this);
	m_onMaterialSet.Invoke(&*newMat);
}

void ModelRenderer::ReloadTexture(std::shared_ptr<Texture> newTex) 
{
	m_onMaterialSet.Invoke(m_model->m_material.get());
}
