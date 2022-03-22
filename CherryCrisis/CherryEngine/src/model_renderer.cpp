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
//	RemoveModel();
}

void ModelRenderer::SetModel(std::shared_ptr<Model> newModel)
{
	m_model = newModel;

	if (m_model)
	{
		m_model->m_onDestroyed.Bind(&ModelRenderer::RemoveModel, this);
		RenderManager::GetInstance()->GenerateFromPipeline<BasicRenderPass>(this);
	}
	else
		RemoveModel();
}

void ModelRenderer::RemoveModel()
{
	// TODO: Add pipeline remove
	if (m_model)
		m_model->m_onDestroyed.Unbind(&ModelRenderer::RemoveModel, this);

	RenderManager::GetInstance()->RemoveFromPipeline<BasicRenderPass>(this);
	m_model = nullptr;
}
