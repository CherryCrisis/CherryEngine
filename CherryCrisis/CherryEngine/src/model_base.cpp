#include "pch.hpp"

#include "model_base.hpp"

#include "threadpool.hpp"
#include "resource_manager.hpp"

#include "model_renderer.hpp"
#include "transform.hpp"
#include "model_loader.hpp"
#include "mesh.hpp"
#include "material.hpp"
#include "scene.hpp"

void ModelBase::Load(std::shared_ptr<ModelBase> modelBase)
{
    std::vector<CCImporter::ImportModelUtils> modelsUtils;
    if (!LoadFromCache(modelBase, modelsUtils))
    {
        CCImporter::ImportModel(*modelBase->GetFilesystemPath(), modelsUtils);
    }

    if (modelsUtils.size() == 0)
        return;

    ResourceManager* resourceManager = ResourceManager::GetInstance();

    std::vector<ModelNode*> modelNodes;

    for (CCImporter::ImportModelUtils& modelUtils : modelsUtils)
    {
        ModelNode* modelNode = new ModelNode();
        std::swap(modelNode->m_baseTRS, modelUtils.modelHeader.m_trs);

        modelNode->m_nodeName = modelUtils.m_modelName;

        if (modelUtils.modelHeader.m_meshHeader.m_hasMesh)
        {
            std::shared_ptr<Mesh> mesh = resourceManager->AddResource<Mesh>(modelUtils.m_meshName.c_str(), 
                false, modelUtils.m_vertices, modelUtils.m_indices);

            std::shared_ptr<Material> material;

            if (modelUtils.modelHeader.m_hasMaterial)
                material = resourceManager->AddResource<Material>(modelUtils.m_materialPath.c_str(), true);

            modelNode->m_mesh = mesh; 
            modelNode->m_material = material;
        }

        modelNodes.push_back(modelNode);
    }

    ModelNode* rootNode = new ModelNode();
    rootNode->m_baseTRS[2] = Vector3::One;

    for (CCImporter::ImportModelUtils& modelUtils : modelsUtils)
    {
        ModelNode* modelNode = modelNodes[modelUtils.modelHeader.m_index];
        for (unsigned int i = 0; i < modelUtils.modelHeader.m_childrenCount; ++i)
            modelNode->m_childrenNode.push_back(modelNodes[modelUtils.m_childrenIndices[i]]);

        if (modelUtils.modelHeader.m_parentIndex >= 0)
        {
            modelNode->m_parentNode = modelNodes[modelUtils.modelHeader.m_parentIndex];
        }
        else
        {
            modelNode->m_parentNode = rootNode;
            rootNode->m_childrenNode.push_back(modelNode);
        }
    }

    modelBase->m_meshCount = static_cast<unsigned int>(modelNodes.size());
    modelBase->m_rootNode = rootNode;
}

bool ModelBase::LoadFromCache(std::shared_ptr<ModelBase> modelBase, std::vector<CCImporter::ImportModelUtils>& models)
{
    std::string fullFilepath(CCImporter::cacheDirectory);
    fullFilepath += modelBase->GetFilesystemPath()->filename().string();
    fullFilepath += CCImporter::cacheExtension;

    FILE* file;
    if (fopen_s(&file, fullFilepath.c_str(), "rb")) //rb = read in binary mode
        return false;

    size_t modelCount;
    fread(&modelCount, sizeof(size_t), 1, file);

    for (int i = 0; i < modelCount; ++i)
    {
        CCImporter::ImportModelUtils model;

        fread(&model.modelHeader, sizeof(CCImporter::ModelHeader), 1, file);

        model.m_modelName.resize(model.modelHeader.m_modelNameSize);
        fread(&model.m_modelName[0], model.modelHeader.m_modelNameSize, 1, file);

        if (model.modelHeader.m_childrenCount)
        {
            model.m_childrenIndices.resize(model.modelHeader.m_childrenCount);
            fread(&model.m_childrenIndices[0], sizeof(unsigned int) * model.modelHeader.m_childrenCount, 1, file);
        }

        if (model.modelHeader.m_meshHeader.m_hasMesh)
        {
            const CCImporter::MeshHeader& meshHeader = model.modelHeader.m_meshHeader;

            model.m_meshName.resize(meshHeader.m_meshNameSize);
            model.m_vertices.resize(meshHeader.m_verticesCount);
            model.m_indices.resize(meshHeader.m_indicesCount);

            fread(&model.m_meshName[0], meshHeader.m_meshNameSize, 1, file);

            fread(&model.m_vertices[0], sizeof(Vertex) * meshHeader.m_verticesCount, 1, file);
            fread(&model.m_indices[0], sizeof(unsigned int) * meshHeader.m_indicesCount, 1, file);
        }

        if (model.modelHeader.m_hasMaterial)
        {
            model.m_materialPath.resize(model.modelHeader.m_materialPathSize);
            fread(&model.m_materialPath[0], model.modelHeader.m_materialPathSize, 1, file);
        }

        models.push_back(model);
    }

    fclose(file);

    return true;
}

void ModelBase::Delete()
{
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

void ModelBase::Reload()
{
    DeleteModelNode(m_rootNode);

    std::vector<CCImporter::ImportModelUtils> modelsUtils;

    CCImporter::ImportModel(*GetFilesystemPath(), modelsUtils);

    if (modelsUtils.size() == 0)
        return;

    ResourceManager* resourceManager = ResourceManager::GetInstance();

    std::vector<ModelNode*> modelNodes;

    std::set<std::shared_ptr<Material>> materialsReloaded; // materials already reloaded
    for (CCImporter::ImportModelUtils& modelUtils : modelsUtils)
    {
        ModelNode* modelNode = new ModelNode();
        std::swap(modelNode->m_baseTRS, modelUtils.modelHeader.m_trs);

        if (modelUtils.modelHeader.m_meshHeader.m_hasMesh)
        {
            std::shared_ptr<Mesh> mesh = resourceManager->GetResource<Mesh>(modelUtils.m_meshName.c_str());

            if (mesh)
            { 
                Resource<Mesh>::ReloadResource(mesh, modelUtils.m_vertices, modelUtils.m_indices);
            }
            else
            {
                mesh = resourceManager->AddResource<Mesh>(modelUtils.m_meshName.c_str(),
                    false, modelUtils.m_vertices, modelUtils.m_indices);
            }

            std::shared_ptr<Material> material = resourceManager->GetResource<Material>(modelUtils.m_materialPath.c_str());

            if (material)
            {
                if (materialsReloaded.find(material) == materialsReloaded.end())
                {
                    Resource<Material>::ReloadResource(material);
                    materialsReloaded.insert(material);
                }
            }
            else
            {
                material = resourceManager->AddResource<Material>(modelUtils.m_materialPath.c_str(), true);
            }

            modelNode->m_mesh = mesh;
            modelNode->m_material = material;
        }

        modelNodes.push_back(modelNode);
    }

    ModelNode* rootNode = new ModelNode();
    rootNode->m_baseTRS[2] = Vector3::One;

    for (CCImporter::ImportModelUtils& modelUtils : modelsUtils)
    {
        ModelNode* modelNode = modelNodes[modelUtils.modelHeader.m_index];
        for (unsigned int i = 0; i < modelUtils.modelHeader.m_childrenCount; ++i)
            modelNode->m_childrenNode.push_back(modelNodes[modelUtils.m_childrenIndices[i]]);

        if (modelUtils.modelHeader.m_parentIndex != -1)
        {
            modelNode->m_parentNode = modelNodes[modelUtils.modelHeader.m_parentIndex];
        }
        else
        {
            modelNode->m_parentNode = rootNode;
            rootNode->m_childrenNode.push_back(modelNode);
        }
    }

    m_meshCount = modelNodes.size();
    m_rootNode = rootNode;
}