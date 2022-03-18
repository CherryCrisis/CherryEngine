#pragma once

#include <vector>
#include <memory>
#include <map>

#include "resource.hpp"
#include "maths.hpp"

#include "renderer.hpp"

class Model;
class Entity;
class Scene;

using namespace CCMaths;

struct ModelNode
{
	ModelNode*				m_parentNode;
	std::vector<ModelNode*> m_childrenNode;
	std::shared_ptr<Model>	m_model;
	Vector3					m_baseTRS[3];
};

class ModelBase : public Resource<ModelBase>
{
private:

	ModelNode* m_rootNode;
	std::vector<std::shared_ptr<Model>>	m_models;

	void GenerateEntitiesRecursive(ModelNode* node, Entity& parentEntity, std::vector<Entity>& entities);
	void DeleteModelNode(ModelNode* modelNode);

public:
	ModelBase(const char* filepath) : Resource(filepath), m_rootNode(nullptr) {}
	~ModelBase();

	static void Load(Ref<ModelBase> modelBase, const char* filepath);

	//Generate entities with modelBase (model instance)
	std::vector<Entity> GenerateEntities(Entity& rootEntity);
};
