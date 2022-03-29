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
	std::string var = "null";
	m_metadatas.SetField<Behaviour*>("transform", m_transform);
	m_metadatas.SetField<std::string>("file", var);

}

void ModelRenderer::ConsumeMetadatas()
{
	Behaviour* b = std::any_cast<Behaviour*>(m_metadatas.m_fields["transform"].m_value);

	m_transform = (Transform*)b;
}

void ModelRenderer::SetModel(std::shared_ptr<Model> newModel)
{
	if (!newModel)
	{
		RemoveModel();
		return;
	}

	m_model = newModel;

	if (m_model.get() != nullptr)
	{
		std::string modelFilepathStr = m_model->GetFilepath();

		if (modelFilepathStr.size() != 0)
			m_metadatas.SetField<std::string>("file", m_model->m_filepath);
	}
	
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

