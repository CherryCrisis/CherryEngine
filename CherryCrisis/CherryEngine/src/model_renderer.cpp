#include "pch.hpp"

#include "model_renderer.hpp"

#include "render_manager.hpp"
#include "resource_manager.hpp"

#include "basic_renderpass.hpp"

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
		RenderManager::GetInstance()->GenerateFromPipeline<BasicRenderPass>(this);
	}
	else
		RemoveModel();
}

void ModelRenderer::RemoveModel()
{
	// TODO: Add pipeline remove
	RenderManager::GetInstance()->RemoveFromPipeline<BasicRenderPass>(this);
}

/*
void ModelRenderer::Deserialize(const char* filepath, const char* modelName) 
{
	ResourceManager* RM = ResourceManager::GetInstance();
	RM->AddResource<ModelBase>(filepath, true);

	std::shared_ptr<Model> model = RM->AddResource<Model>(modelName, true);

}*/