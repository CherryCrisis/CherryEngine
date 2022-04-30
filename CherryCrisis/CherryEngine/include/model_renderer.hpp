#pragma once

#include "cherry_macros.hpp"

#include "behaviour.hpp"

#include "renderer.hpp"

#include "model_base.hpp"

#include "property.hpp"

class Transform;
class Material;
class Texture;

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
	std::string m_modelPath;
	ModelRenderer();
	ModelRenderer(CCUUID& id);
	~ModelRenderer();

	void Initialize();
	void BindToSignals() override;

	void SetModelFromPath(const char* modelPath);
	const char* GetModelPath();
	void OnModelLoaded(std::shared_ptr<Model> model);
	void SetModel(std::shared_ptr<Model> newModel);
	void RemoveModel();

	void SetMaterial(Material* newMat);
	void ReloadTexture(std::shared_ptr<Texture> newTex);

	void SubscribeToPipeline(ARenderingPipeline* pipeline) override;
	void UnsubscribeToPipeline(ARenderingPipeline* pipeline) override;
	void OnCellAdded(Cell* newCell);
	void OnCellRemoved(Cell* newCell);

	CCProperty::CopyProperty<ModelRenderer, const char*> m_ModelPath { this, &ModelRenderer::SetModelFromPath, &ModelRenderer::GetModelPath };

	Event<Material*> m_onMaterialSet;
};