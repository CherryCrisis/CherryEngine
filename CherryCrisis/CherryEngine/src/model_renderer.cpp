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
	RenderManager::GetInstance()->RemoveFromPipeline<BasicRenderPass>(this);
}

void ModelRenderer::PopulateMetadatas() 
{
	m_metadatas.SetField<Behaviour*>("transform", m_transform);
}

void ModelRenderer::SetModel(std::shared_ptr<Model> newModel)
{
	m_model = newModel;

	if (m_model)
	{
		m_model->m_onDestroyed.Bind(&ModelRenderer::RemoveModel, this);
		m_metadatas.SetField<std::string>("filepath", m_model->m_modelBasePath);

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

/*
void ModelRenderer::Deserialize(const char* filepath, const char* modelName) 
{
	ResourceManager* RM = ResourceManager::GetInstance();
	RM->AddResource<ModelBase>(filepath, true);

	std::shared_ptr<Model> model = RM->AddResource<Model>(modelName, true);

}*/
