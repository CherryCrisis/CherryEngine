#pragma once

#include "cherry_macros.hpp"

#include "behaviour.hpp"

#include "renderer.hpp"

#include "model_base.hpp"

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
	std::string model_path = "null";
	ModelRenderer();
	ModelRenderer(CCUUID& id);
	~ModelRenderer();

	void Initialize();
	void BindToSignals() override;

	void SetModel(std::shared_ptr<Model> newModel);
	void RemoveModel();

	void OnSetMaterial(Material* newMat);
	void OnReloadMaterial(std::shared_ptr<Material> material);
	void ReloadTexture(std::shared_ptr<Texture> newTex);

	void SubscribeToPipeline(ARenderingPipeline* pipeline) override;
	void UnsubscribeToPipeline(ARenderingPipeline* pipeline) override;
	void OnCellAdded(Cell* newCell);
	void OnCellRemoved(Cell* newCell);

	Event<Material*> m_onMaterialSet;
};