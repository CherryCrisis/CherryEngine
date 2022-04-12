#pragma once

#include <vector>
#include <memory>
#include <map>

#include "resource.hpp"
#include "maths.hpp"

#include "renderer.hpp"

#include "cherry_macros.hpp"

class Model;
class Entity;
class Scene;

using namespace CCMaths;

namespace CCImporter
{
	struct ImportModelUtils;
}

struct ModelNode
{
	ModelNode*				m_parentNode;
	std::vector<ModelNode*> m_childrenNode;
	std::shared_ptr<Model>	m_model;
	Vector3					m_baseTRS[3];
};

class CCENGINE_API ModelBase : public Resource<ModelBase>
{
private:

	ModelNode* m_rootNode;
	std::vector<std::shared_ptr<Model>>	m_models;

	std::string m_relativePath;

	void GenerateEntitiesRecursive(ModelNode* node, Entity* parentEntity, std::vector<Entity*>& entities);
	void DeleteModelNode(ModelNode* modelNode);

public:
	ModelBase(const char* filename) : Resource(filename), m_rootNode(nullptr) {}
	~ModelBase() = default;

	static void Load(std::shared_ptr<ModelBase> modelBase, const char* relativePath);
	static bool LoadFromCache(std::shared_ptr<ModelBase> modelBase, std::vector<CCImporter::ImportModelUtils>& models);

	void Delete() override;
	void Reload();

	//Generate entities with modelBase (model instance)
	std::vector<Entity*> GenerateEntities(Entity* rootEntity);
};
