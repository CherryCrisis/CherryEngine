#pragma once

#include "mesh_renderer.hpp"


enum class EMeshShape : int;

class CCENGINE_API ShapeRenderer : public MeshRenderer
{
private:
	void PopulateMetadatas() override;

	EMeshShape m_meshType = (EMeshShape)0u;

public:
	ShapeRenderer();
	ShapeRenderer(CCUUID& id);
	~ShapeRenderer();

	void Initialize() override;
	void BindToSignals() override;

	void SetType(int type);
	int  GetType();

	void OnMeshLoaded(std::shared_ptr<Mesh> newMesh);
	void OnCellAdded(Cell* newCell);
	void OnCellRemoved(Cell* newCell);

	CCProperty::CopyProperty<ShapeRenderer, int> m_MeshType{ this, &ShapeRenderer::SetType, &ShapeRenderer::GetType };
};