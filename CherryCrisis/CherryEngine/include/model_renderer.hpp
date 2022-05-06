#pragma once

#include "cherry_macros.hpp"

#include "behaviour.hpp"

#include "renderer.hpp"

#include "model_base.hpp"

#include "property.hpp"

class Transform;
class Material;
class Mesh;
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

	Transform*					m_transform = nullptr;
	std::shared_ptr<Mesh>		m_mesh = nullptr;
	std::shared_ptr<Material>	m_material = nullptr;

	std::string					m_meshPath;
	std::string					m_materialPath;

	ModelRenderer();
	ModelRenderer(CCUUID& id);
	~ModelRenderer();

	void Initialize();
	void BindToSignals() override;

	void LoadModelFromPath(const char* modelPath);
	const char* GetModelPath();

	void SetMeshFromPath(const char* meshPath);
	const char* GetMeshPath();

	void OnMeshLoaded(std::shared_ptr<Mesh> newMesh);
	void SetMesh(std::shared_ptr<Mesh> newMesh);
	void RemoveMesh();

	void SetMaterialFromPath(const char* meshPath);
	const char* GetMaterialPath();

	void OnMaterialLoaded(std::shared_ptr<Material> newMat);
	void OnMaterialReloaded(std::shared_ptr<Material> newMat);
	void SetMaterial(std::shared_ptr<Material> newMat);
	void RemoveMaterial();

	//void OnReloadMaterial(std::shared_ptr<Material> material);
	//void ReloadTexture(std::shared_ptr<Texture> newTex);

	void SubscribeToPipeline(ARenderingPipeline* pipeline) override;
	void UnsubscribeToPipeline(ARenderingPipeline* pipeline) override;
	void OnCellAdded(Cell* newCell);
	void OnCellRemoved(Cell* newCell);
	
	CCProperty::CopyProperty<ModelRenderer, const char*> m_ModelPath{ this, &ModelRenderer::LoadModelFromPath, &ModelRenderer::GetModelPath };
	CCProperty::CopyProperty<ModelRenderer, const char*> m_MeshPath{ this, &ModelRenderer::SetMeshFromPath, &ModelRenderer::GetMeshPath };
	CCProperty::CopyProperty<ModelRenderer, const char*> m_MaterialPath { this, &ModelRenderer::SetMaterialFromPath, &ModelRenderer::GetMaterialPath };
};