#include "pch.hpp"

#include "model_renderer.hpp"

#include "render_manager.hpp"
#include "resource_manager.hpp"

#include "shadow_renderpass.hpp"
#include "basic_renderpass.hpp"
#include "entity.hpp"
#include "model.hpp"
#include "picking_renderpass.hpp"

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
	RemoveModel();
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
	SubscribeToRenderPass();
}

void ModelRenderer::RemoveModel() 
{
	// TODO: Add pipeline remove
	if (m_model)
		m_model->m_OnDeleted.Unbind(&ModelRenderer::RemoveModel, this);

	UnsubscribeToRenderPass();
	m_model = nullptr;
}

void ModelRenderer::SubscribeToRenderPass()
{
	RenderManager::GetInstance()->SubscribeToPipeline<ShadowRenderPass>(this);
	RenderManager::GetInstance()->SubscribeToPipeline<BasicRenderPass>(this);
	RenderManager::GetInstance()->SubscribeToPipeline<PickingRenderPass>(this);
}

void ModelRenderer::UnsubscribeToRenderPass()
{
	RenderManager::GetInstance()->UnsubscribeToPipeline<ShadowRenderPass>(this);
	RenderManager::GetInstance()->UnsubscribeToPipeline<BasicRenderPass>(this);
	RenderManager::GetInstance()->UnsubscribeToPipeline<PickingRenderPass>(this);
}

void ModelRenderer::Initialize() 
{
	if (m_model)
		SubscribeToRenderPass();

	GetHost().m_OnAwake.Unbind(&ModelRenderer::Initialize, this);
}

void ModelRenderer::BindToSignals() 
{
	GetHost().m_OnAwake.Bind(&ModelRenderer::Initialize, this);
}