#include "pch.hpp"

#include "model_renderer.hpp"

#include "render_manager.hpp"
#include "resource_manager.hpp"

#include "basic_renderpass.hpp"
#include "entity.hpp"
#include "model.hpp"

ModelRenderer::ModelRenderer(Entity& owner)
	: Behaviour(owner)
{
	PopulateMetadatas();
}

ModelRenderer::~ModelRenderer()
{
	RemoveModel();
}

void ModelRenderer::PopulateMetadatas()
{
	m_metadatas.SetField<Behaviour*>("transform", m_transform);
}

void ModelRenderer::SetModel(std::shared_ptr<Model> newModel)
{
	if (!newModel)
	{
		RemoveModel();
		return;
	}

	m_model = newModel;

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
	RenderManager::GetInstance()->GenerateFromPipeline<BasicRenderPass>(this);
}

void ModelRenderer::UnsubscribeToRenderPass()
{
	RenderManager::GetInstance()->RemoveFromPipeline<BasicRenderPass>(this);
}

/*
void ModelRenderer::Deserialize(const char* filepath, const char* modelName) 
{
	ResourceManager* RM = ResourceManager::GetInstance();
	RM->AddResource<ModelBase>(filepath, true);

	std::shared_ptr<Model> model = RM->AddResource<Model>(modelName, true);

}*/
