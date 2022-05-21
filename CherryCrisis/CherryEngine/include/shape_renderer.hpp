#pragma once

#include <cherry_macros.hpp>

#include "behaviour.hpp"
#include "model.hpp"
#include "property.hpp"
#include "renderer.hpp"

class Cell;
class Material;
class Mesh;
class Texture;
class Transform;


class CCENGINE_API ShapeRenderer : public Behaviour, public ARenderer
{
private:
	static int count;

	void PopulateMetadatas() override;

public:
	bool m_initialized = false;
	int m_id = -1;

	std::string					m_materialPath;
	std::string					m_meshPath;

	std::shared_ptr<Material>	m_material = nullptr;
	std::shared_ptr<Mesh>		m_mesh = nullptr;

	Transform*					m_transform = nullptr;

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