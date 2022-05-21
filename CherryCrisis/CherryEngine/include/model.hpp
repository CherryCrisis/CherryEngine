#pragma once

#include <map>
#include <memory>
#include <vector>

#include <cherry_macros.hpp>

#include "maths.hpp"
#include "renderer.hpp"
#include "resource.hpp"

class Mesh;
class Material;
class Entity;
class Scene;

using namespace CCMaths;


namespace CCImporter
{
	struct ImportModelUtils;
}

struct ModelNode
{
	Vector3						m_baseTRS[3];
	
	std::string					m_nodeName;
	std::vector<ModelNode*>		m_childrenNode;
	std::shared_ptr<Mesh>		m_mesh;
	std::shared_ptr<Material>	m_material;

	ModelNode*					m_parentNode;
};

class CCENGINE_API Model : public Resource<Model>
{
private:
	unsigned int	m_meshCount = 0;

	ModelNode*		m_rootNode = nullptr;

	void DeleteModelNode(ModelNode* modelNode);

public:
	Model(const char* filename) : Resource(filename) {}
	~Model() = default;

	size_t GetMeshCount() { return m_meshCount; }
	ModelNode* GetRootNode() { return m_rootNode; }

	static void Load(std::shared_ptr<Model> modelBase);
	static bool LoadFromCache(std::shared_ptr<Model> modelBase, std::vector<CCImporter::ImportModelUtils>& models);

	void Delete() override;
	void Reload();
};
