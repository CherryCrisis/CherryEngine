#pragma once

#include "mesh_renderer.hpp"


class CCENGINE_API ModelRenderer : public MeshRenderer
{
private:
	std::string					m_meshPath;

	void PopulateMetadatas() override;

public:
	ModelRenderer();
	ModelRenderer(CCUUID& id);
	~ModelRenderer();

	void Initialize() override;
	void BindToSignals() override;

	void SetMeshFromPath(std::string meshPath);
	std::string GetMeshPath();

	void LoadModelFromPath(std::string modelPath);
	std::string GetModelPath();

	void OnMeshLoaded(std::shared_ptr<Mesh> newMesh);
	void OnCellAdded(Cell* newCell);
	void OnCellRemoved(Cell* newCell);

	CCProperty::CopyProperty<ModelRenderer, std::string> m_MeshPath{ this, &ModelRenderer::SetMeshFromPath, &ModelRenderer::GetMeshPath };
	CCProperty::CopyProperty<ModelRenderer, std::string> m_ModelPath{ this, &ModelRenderer::LoadModelFromPath, &ModelRenderer::GetModelPath };
};