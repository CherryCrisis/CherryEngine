#pragma once

#include "cherry_macros.hpp"

#include "behaviour.hpp"

#include "renderer.hpp"

#include "model_base.hpp"

class Transform;

class CCENGINE_API ModelRenderer : public Behaviour, public ARenderer
{
private:
	void PopulateMetadatas() override;
public:


	Transform* m_transform = nullptr;
	std::shared_ptr<Model> m_model;

	ModelRenderer(Entity& owner);
	ModelRenderer() { PopulateMetadatas(); };
	~ModelRenderer();

	void SetModel(std::shared_ptr<Model> newModel);
	void RemoveModel();
	void SubscribeToRenderPass();
	void UnsubscribeToRenderPass();
};