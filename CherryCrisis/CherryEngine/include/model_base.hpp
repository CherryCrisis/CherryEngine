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

class ModelBase : public ResourceMultithread
{
private:
	ModelBase(const char* filepath) : ResourceMultithread(filepath), m_rootNode(nullptr) {}

	ModelNode* m_rootNode;
	std::vector<std::shared_ptr<Model>>	m_models;

	void GenerateEntitiesRecursive(ModelNode* node, Entity& parentEntity, std::vector<Entity>& entities);
	void DeleteModelNode(ModelNode* modelNode);

public:
	~ModelBase();

	static Ref<ModelBase> Create(const char* filepath);

	//Generate entities with modelBase (model instance)
	std::vector<Entity> GenerateEntities(Entity& rootEntity);
};
