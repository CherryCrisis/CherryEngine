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
	static int count;
public:
	int m_id = -1;

	// TODO: Remove this
	bool m_initialized = false;

	Transform* m_transform = nullptr;
	std::shared_ptr<Model> m_model;
	std::string model_path = "null";
	ModelRenderer();
	ModelRenderer(CCUUID& id);
	~ModelRenderer();

	void Initialize();
	void BindToSignals() override;

	void SetModel(std::shared_ptr<Model> newModel);
	void RemoveModel();

	void SubscribeToPipeline(ARenderingPipeline* pipeline) override;
	void UnsubscribeToPipeline(ARenderingPipeline* pipeline) override;
};