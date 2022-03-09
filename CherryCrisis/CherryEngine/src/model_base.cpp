#include "pch.hpp"

#include "model_base.hpp"

#include "threadpool.hpp"

#include "model_loader.hpp"
#include "scene.hpp"

Resource::Ref<ModelBase> ModelBase::Create(const char* filepath)
{
    ModelBase* modelBase = new ModelBase(filepath);

    ThreadPool* threadpool = ThreadPool::GetInstance();

    std::unique_ptr<CCFunction::AFunction> function = 
        CCFunction::BindFunction(LoadModel, modelBase->filepath.c_str(), &modelBase->m_rootNode, modelBase->m_models);

    std::unique_ptr<CCFunction::AFunction> onFinished =
        CCFunction::BindFunction(&ResourceMultithread::ResourceLoaded, (ResourceMultithread*)modelBase);

    threadpool->CreateTask(function, onFinished, EChannelTask::Multithread);

    //LoadModel(&test, &modelBase->m_rootNode, modelBase->m_models);

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

std::vector<Entity> ModelBase::GenerateEntities(Entity& rootEntity)
{
    std::vector<Entity> entities;

    if (m_models.size() != 0 && m_rootNode != nullptr)
        GenerateEntitiesRecursive(m_rootNode, rootEntity, entities);

    return entities;
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
