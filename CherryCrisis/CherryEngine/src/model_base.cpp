#include "pch.hpp"

#include "model_base.hpp"

#include "model_loader.hpp"
#include "entity.hpp"

#include "model_renderer.hpp"
#include "transform.hpp"

Resource::Ref<ModelBase> ModelBase::Create(const char* filepath)
{
	ModelBase* modelBase = new ModelBase(filepath);
	CCModelLoader::LoadModel(filepath, &modelBase->m_rootNode, modelBase->m_models);

	return Ref<ModelBase>(modelBase);
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

std::vector<Entity*>& ModelBase::GenerateEntities(Entity* rootEntity)
{
    std::vector<Entity*> entities;

    if (m_models.size() != 0 && m_rootNode != nullptr)
        GenerateEntitiesRecursive(m_rootNode, rootEntity, entities);

    return entities;
}

void ModelBase::GenerateEntitiesRecursive(ModelNode* node, Entity* parentEntity, std::vector<Entity*>& entities)
{
    Entity* entity = new Entity();
    ModelRenderer* modelRdr = entity->AddBehaviour<ModelRenderer>();
    Transform* entityTransform = entity->AddBehaviour<Transform>();

    modelRdr->m_transform = entityTransform;
    modelRdr->SetModel(node->m_model);

    entityTransform->SetPosition(node->m_baseTRS[0]);
    entityTransform->SetRotation(node->m_baseTRS[1]);
    entityTransform->SetScale(node->m_baseTRS[2]);

    Transform* parentTransform = parentEntity->GetOrAddBehaviour<Transform>();

    entityTransform->SetParent(parentTransform);

    entities.push_back(entity);

    for (ModelNode* childNode : node->m_childrenNode)
        GenerateEntitiesRecursive(childNode, entity, entities);

    //parentTransform->AddChildren(entityTransform);
}