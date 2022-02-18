#include "modelBase.hpp"

#include "modelLoader.hpp"
#include "entity.hpp"

Resource* ModelBase::Create(const char* filepath)
{
	ModelBase* modelBase = new ModelBase(filepath);
	CCModelLoader::LoadModel(filepath, &modelBase->m_rootNode, modelBase->m_models);

	return modelBase;
}

ModelBase::~ModelBase()
{
	m_models.clear();
	DeleteModelNode(m_rootNode);
}

void ModelBase::DeleteModelNode(ModelNode* modelNode)
{
	for (int i = 0; i < modelNode->m_childrenNode.size(); ++i)
		DeleteModelNode(modelNode->m_childrenNode[i]);

	delete modelNode;
}

std::vector<Entity> ModelBase::GenerateEntities(Entity& rootEntity)
{
    std::vector<Entity> entities;

    if (m_models.size() != 0 && m_rootNode != nullptr)
        GenerateEntitiesRecursive(m_rootNode, rootEntity, entities);

    return std::move(entities);
}

void ModelBase::GenerateEntitiesRecursive(ModelNode* node, Entity& parentEntity, std::vector<Entity>& entities)
{
    Entity entity;
    entity.m_modelRenderer->SetModel(node->m_model);

    entity.m_transform->SetPosition(node->m_baseTRS[0]);
    entity.m_transform->SetRotation(node->m_baseTRS[1]);
    entity.m_transform->SetScale(node->m_baseTRS[2]);
    entity.m_transform->SetParent(parentEntity.m_transform);

    entities.push_back(entity);

    for (ModelNode* childNode : node->m_childrenNode)
        GenerateEntitiesRecursive(childNode, entity, entities);

    parentEntity.m_transform->AddChildren(entity.m_transform);
}
