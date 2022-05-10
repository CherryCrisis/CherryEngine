#pragma once

#include <vector>
#include <memory>
#include <map>

#include "resource.hpp"
#include "maths.hpp"

#include "renderer.hpp"

#include "cherry_macros.hpp"

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
	std::string					m_nodeName;
	ModelNode*					m_parentNode;
	std::vector<ModelNode*>		m_childrenNode;
	std::shared_ptr<Mesh>		m_mesh;
	std::shared_ptr<Material>	m_material;
	Vector3						m_baseTRS[3];
};

class CCENGINE_API ModelBase : public Resource<ModelBase>
{
private:
	ModelNode*		m_rootNode = nullptr;
	unsigned int	m_meshCount = 0;

	void DeleteModelNode(ModelNode* modelNode);

public:
	ModelBase(const char* filename) : Resource(filename) {}
	~ModelBase() = default;

	size_t GetMeshCount() { return m_meshCount; }
	ModelNode* GetRootNode() { return m_rootNode; }

	static void Load(std::shared_ptr<ModelBase> modelBase);
	static bool LoadFromCache(std::shared_ptr<ModelBase> modelBase, std::vector<CCImporter::ImportModelUtils>& models);

	void Delete() override;
	void Reload();
};
