#include "pch.hpp"

#include "model_renderer.hpp"

#include "render_manager.hpp"
#include "resource_manager.hpp"

#include "basic_renderpass.hpp"
#include "model.hpp"
#include "entity.hpp"

ModelRenderer::ModelRenderer(Entity& owner)
	: Behaviour(owner)
{
}

ModelRenderer::~ModelRenderer()
{
	RemoveModel();
}

void ModelRenderer::SetModel(std::shared_ptr<Model> newModel)
{
	if (!newModel)
	{
		RemoveModel();
		return;
	}

	m_model = newModel;

	m_model->m_onDestroyed.Bind(&ModelRenderer::RemoveModel, this);
	SubscribeToRenderPass();
}

void ModelRenderer::RemoveModel()
{
	// TODO: Add pipeline remove
	if (m_model)
		m_model->m_onDestroyed.Unbind(&ModelRenderer::RemoveModel, this);

	UnsubscribeToRenderPass();
	m_model = nullptr;
}

void ModelRenderer::SubscribeToRenderPass()
{
	RenderManager::GetInstance()->GenerateFromPipeline<BasicRenderPass>(this);
}

void ModelRenderer::UnsubscribeToRenderPass()
{
	RenderManager::GetInstance()->RemoveFromPipeline<BasicRenderPass>(this);
}