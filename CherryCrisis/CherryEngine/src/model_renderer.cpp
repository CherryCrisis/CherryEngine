#include "pch.hpp"

#include "model_renderer.hpp"

#include "render_manager.hpp"
#include "resource_manager.hpp"

#include "basic_renderpass.hpp"

#include "entity.hpp"

ModelRenderer::ModelRenderer()
	: Behaviour()
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
		RenderManager::instance()->GenerateFromPipeline<BasicRenderPass>(this);
	}
	else
		RemoveModel();
}

void ModelRenderer::RemoveModel()
{
	// TODO: Add pipeline remove
	RenderManager::instance()->RemoveFromPipeline<BasicRenderPass>(this);
}

void ModelRenderer::Draw()
{
	//RenderManager::instance()->ConsumeFromPipeline<BasicSubPipeline>(this);
}