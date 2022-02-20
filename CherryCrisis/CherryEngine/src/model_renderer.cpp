#include "model_renderer.hpp"

#include "render_manager.hpp"
#include "resourceManager.hpp"

#include "basic_subpipeline.hpp"

#include "entity.hpp"

ModelRenderer::ModelRenderer()
	: Behaviour()
{
}

ModelRenderer::~ModelRenderer()
{
	RemoveModel();
}

void ModelRenderer::SetModel(std::shared_ptr<Model> newModel)
{
	if (!m_model)
		RenderManager::instance()->SubscribeRenderer(this);

	m_model = newModel;

	if (m_model)
	{
		RenderManager::instance()->GenerateFromPipeline<BasicSubPipeline>(this);
	}
	else
		RemoveModel();
}

void ModelRenderer::RemoveModel()
{
	RenderManager::instance()->UnsubscribeRenderer(this);
}

void ModelRenderer::Draw()
{
	RenderManager::instance()->ConsumeFromPipeline<BasicSubPipeline>(this);
}