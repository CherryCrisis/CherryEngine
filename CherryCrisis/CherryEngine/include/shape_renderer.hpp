#pragma once

#include "cherry_macros.hpp"

#include "behaviour.hpp"

#include "renderer.hpp"

#include "model.hpp"

#include "property.hpp"

class Transform;
class Material;
class Mesh;
class Texture;
class Cell;

class CCENGINE_API ShapeRenderer : public Behaviour, public ARenderer
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

	ShapeRenderer();
	ShapeRenderer(CCUUID& id);
	~ShapeRenderer();

	void Initialize();
	void BindToSignals() override;

	void LoadModelFromPath(std::string modelPath);
	std::string GetModelPath();

	void SetMeshFromPath(std::string meshPath);
	std::string GetMeshPath();

	void OnMeshLoaded(std::shared_ptr<Mesh> newMesh);
	void SetMesh(std::shared_ptr<Mesh> newMesh);
	void RemoveMesh();

	void SetMaterialFromPath(std::string meshPath);
	std::string GetMaterialPath();

	void OnMaterialLoaded(std::shared_ptr<Material> newMat);
	void OnMaterialReloaded(std::shared_ptr<Material> newMat);
	void SetMaterial(std::shared_ptr<Material> newMat);
	void RemoveMaterial();

	void SubscribeToPipeline(ARenderingPipeline* pipeline) override;
	void UnsubscribeToPipeline(ARenderingPipeline* pipeline) override;
	void OnCellAdded(Cell* newCell);
	void OnCellRemoved(Cell* newCell);

	CCProperty::CopyProperty<ShapeRenderer, std::string> m_MeshPath{ this, &ShapeRenderer::SetMeshFromPath, &ShapeRenderer::GetMeshPath };
	CCProperty::CopyProperty<ShapeRenderer, std::string> m_MaterialPath{ this, &ShapeRenderer::SetMaterialFromPath, &ShapeRenderer::GetMaterialPath };
};