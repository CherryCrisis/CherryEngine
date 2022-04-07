#include "pch.hpp"

#include "model_base.hpp"

#include "threadpool.hpp"
#include "resource_manager.hpp"

#include "model_renderer.hpp"
#include "transform.hpp"
#include "model_loader.hpp"
#include "mesh.hpp"
#include "scene.hpp"

void ModelBase::Load(std::shared_ptr<ModelBase> modelBase)
{
    std::vector<CCModelLoader::ImportModelUtils> modelsUtils;
    if (!LoadFromCache(modelBase, modelsUtils))
    {
        CCModelLoader::ImportModel(modelBase->GetFilepath(), modelsUtils);
    }

    ResourceManager* resourceManager = ResourceManager::GetInstance();

    std::vector<ModelNode*> modelNodes;

    for (CCModelLoader::ImportModelUtils& modelUtils : modelsUtils)
    {
        ModelNode* modelNode = new ModelNode();
        std::swap(modelNode->m_baseTRS, modelUtils.modelHeader.m_trs);

        if (modelUtils.modelHeader.m_hasMesh)
        {
            std::string modelName(std::to_string(modelUtils.modelHeader.m_modelId));
            std::string meshName(std::to_string(modelUtils.modelHeader.m_meshId));

            std::shared_ptr<Mesh> mesh = resourceManager->AddResource<Mesh>(meshName.c_str(), true, modelUtils.m_vertices, modelUtils.m_indices);
            std::shared_ptr<Model> model = resourceManager->AddResource<Model>(modelName.c_str(), true, mesh);

            modelNode->m_model = model;
            modelBase->m_models.push_back(model);
        }

        modelNodes.push_back(modelNode);
    }

    /*for (CCModelLoader::ImportModelUtils& modelUtils : modelsUtils)
    {
        ModelNode* modelNode = new ModelNode();
        std::swap(modelNode->m_baseTRS, modelUtils.modelHeader.m_trs);
        modelNode->m_model = modelBase->m_models[modelUtils.modelHeader.m_index];
        modelNodes.push_back(modelNode);
    }*/

    for (CCModelLoader::ImportModelUtils& modelUtils : modelsUtils)
    {
        ModelNode* modelNode = modelNodes[modelUtils.modelHeader.m_index];
        for (int i = 0; i < modelUtils.modelHeader.m_childrenCount; ++i)
            modelNode->m_childrenNode.push_back(modelNodes[modelUtils.m_childrenIndices[i]]);

        if (modelUtils.modelHeader.m_parentIndex != -1)
            modelNode->m_parentNode = modelNodes[modelUtils.modelHeader.m_parentIndex];
    }

    modelBase->m_rootNode = modelNodes[0];

}

bool ModelBase::LoadFromCache(std::shared_ptr<ModelBase> modelBase, std::vector<CCModelLoader::ImportModelUtils>& models)
{
    std::string fileIDStr = modelBase->GetFilepath();
    fileIDStr += ".ccfile";

    FILE* file;
    if (fopen_s(&file, fileIDStr.c_str(), "rb")) //rb = read in binary mode
    {
        return false;
    }

    size_t modelCount;
    fread(&modelCount, sizeof(size_t), 1, file);

    for (int i = 0; i < modelCount; ++i)
    {
        CCModelLoader::ImportModelUtils model;

        fread(&model.modelHeader, sizeof(CCModelLoader::ModelHeader), 1, file);

        if (model.modelHeader.m_childrenCount)
        {
            model.m_childrenIndices.resize(model.modelHeader.m_childrenCount);
            fread(&model.m_childrenIndices[0], sizeof(unsigned int) * model.modelHeader.m_childrenCount, 1, file);
        }

        if (model.modelHeader.m_hasMesh)
        {
            model.m_vertices.resize(model.modelHeader.m_verticesCount);
            model.m_indices.resize(model.modelHeader.m_indicesCount);

            fread(&model.m_vertices[0], sizeof(Vertex) * model.modelHeader.m_verticesCount, 1, file);
            fread(&model.m_indices[0], sizeof(unsigned int) * model.modelHeader.m_indicesCount, 1, file);
        }

        models.push_back(model);
    }

    return true;
}

void ModelBase::Delete()
{
    for (std::shared_ptr<Model>& model : m_models)
        model->DeleteResource();

    m_models.clear();
    DeleteModelNode(m_rootNode);
}

void ModelBase::DeleteModelNode(ModelNode* modelNode)
{
    if (!modelNode)
        return;

	for (int i = 0; i < modelNode->m_childrenNode.size(); ++i)
		DeleteModelNode(modelNode->m_childrenNode[i]);

	delete modelNode;
}

std::vector<Entity*> ModelBase::GenerateEntities(Entity* rootEntity)
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

void ModelBase::Reload()
{
    //CCModelLoader::ReloadModel(GetFilepath(), &m_rootNode, m_models);
}