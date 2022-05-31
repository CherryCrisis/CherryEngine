#pragma once

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

enum class ERendererType : int
{
	MODEL,
	SHAPE,
};


class CCENGINE_API MeshRenderer : public Behaviour, public ARenderer
{
private:
	virtual void PopulateMetadatas() override;

	static int count;

public:
	bool	m_initialized = false;
	int		m_id = -1;

	std::string					m_materialPath;

	std::shared_ptr<Material>	m_material = nullptr;
	std::shared_ptr<Mesh>		m_mesh = nullptr;

	ERendererType				m_rendererType;

	Transform* m_transform = nullptr;

	CCMaths::Vector3	m_sliceCentre = CCMaths::Vector3::Zero;
	CCMaths::Vector3	m_sliceNormal = CCMaths::Vector3::Zero;
	bool				m_isSlice = false;

	MeshRenderer(ERendererType rendererType);
	MeshRenderer(ERendererType rendererType, CCUUID& id);

	virtual ~MeshRenderer() = default;

	virtual void Initialize() {}
	virtual void BindToSignals() override {}

	virtual void OnMeshLoaded(std::shared_ptr<Mesh> newMesh);
	virtual void SetMesh(std::shared_ptr<Mesh> newMesh);
	virtual void RemoveMesh();

	virtual void SetMaterialFromPath(std::string meshPath);
	virtual std::string GetMaterialPath();

	virtual void OnMaterialLoaded(std::shared_ptr<Material> newMat);
	virtual void OnMaterialReloaded(std::shared_ptr<Material> newMat);
	virtual void SetMaterial(std::shared_ptr<Material> newMat);
	virtual void RemoveMaterial();

	virtual void Copy(Behaviour* copy) override;

	virtual void SubscribeToPipeline(ARenderingPipeline* pipeline) override;
	virtual void UnsubscribeToPipeline(ARenderingPipeline* pipeline) override;
	virtual void OnCellAdded(Cell* newCell) {}
	virtual void OnCellRemoved(Cell* newCell) {}

	CCProperty::CopyProperty<MeshRenderer, std::string> m_MaterialPath{ this, &MeshRenderer::SetMaterialFromPath, &MeshRenderer::GetMaterialPath };
};