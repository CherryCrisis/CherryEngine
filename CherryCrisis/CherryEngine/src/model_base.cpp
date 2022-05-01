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

        if (modelUtils.modelHeader.m_meshHeader.m_hasMesh)
        {
            std::shared_ptr<Mesh> mesh = resourceManager->AddResource<Mesh>(modelUtils.m_meshName.c_str(), 
                false, modelUtils.m_vertices, modelUtils.m_indices);

            MaterialArgs materialArgs
            { 
                .m_materialHeader = &modelUtils.modelHeader.m_materialHeader,
                .m_texturesPath = &modelUtils.m_texturesPathCstr,
                .m_textureType = &modelUtils.m_texturesType,
            };

            std::shared_ptr<Material> material = resourceManager->AddResource<Material>(modelUtils.m_materialName.c_str(), true, materialArgs);
            
            std::shared_ptr<Model> model = resourceManager->AddResource<Model>(modelUtils.m_modelName.c_str(), true, mesh, material, modelBase);

            modelNode->m_model = model;
            modelBase->m_models.push_back(model);
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
        if (i == 2)
            int j = 0;

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

        if (model.modelHeader.m_materialHeader.m_hasMaterial)
        {
            const CCImporter::MaterialHeader& materialHeader = model.modelHeader.m_materialHeader;

            model.m_materialName.resize(materialHeader.m_materialNameSize);
            fread(&model.m_materialName[0], materialHeader.m_materialNameSize, 1, file);

            unsigned int texturesCount = materialHeader.m_texturesCount;
            if (texturesCount)
            {
                model.m_texturesPathSize.resize(texturesCount);
                model.m_texturesType.resize(texturesCount);

                fread(&model.m_texturesPathSize[0], texturesCount * sizeof(unsigned int), 1, file);
                fread(&model.m_texturesType[0], texturesCount * sizeof(unsigned int), 1, file);
                
                for (unsigned int i = 0; i < materialHeader.m_texturesCount; ++i)
                {
                    std::string texturePath;
                    texturePath.resize(model.m_texturesPathSize[i]);

                    fread(&texturePath[0], model.m_texturesPathSize[i], 1, file);
                    model.m_texturesPathCstr.push_back(std::move(texturePath));
                }
            }
        }

        models.push_back(model);
    }

    fclose(file);

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

void ModelBase::Reload()
{
    m_models.clear();
    DeleteModelNode(m_rootNode);

    std::vector<CCImporter::ImportModelUtils> modelsUtils;

    CCImporter::ImportModel(*GetFilesystemPath(), modelsUtils);

    if (modelsUtils.size() == 0)
        return;

    ResourceManager* resourceManager = ResourceManager::GetInstance();

    std::vector<ModelNode*> modelNodes;

    std::vector<std::shared_ptr<Material>> materialsReloaded; // materials already reloaded
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

            MaterialArgs materialArgs
            {
                .m_materialHeader = &modelUtils.modelHeader.m_materialHeader,
                .m_texturesPath = &modelUtils.m_texturesPathCstr,
                .m_textureType = &modelUtils.m_texturesType,
            };

            std::shared_ptr<Material> material = resourceManager->GetResource<Material>(modelUtils.m_materialName.c_str());

            if (material)
            {
                bool reloaded = false;
                for (const auto materialReloaded : materialsReloaded)
                {
                    if (materialReloaded == material)
                    {
                        reloaded = true;
                        break;
                    }
                }

                if (!reloaded)
                {
                    Resource<Material>::ReloadResource(material, materialArgs);
                    materialsReloaded.push_back(material);
                }
            }
            else
            {
                material = resourceManager->AddResource<Material>(modelUtils.m_materialName.c_str(), true, materialArgs);
            }
                
            

            std::shared_ptr<Model> model = resourceManager->GetResource<Model>(modelUtils.m_modelName.c_str());

            if (model)
            {
                Resource<Model>::ReloadResource(model, mesh, material);
            }
            else
            {
                model = resourceManager->AddResource<Model>(modelUtils.m_modelName.c_str(), true, mesh, material);
            }



            modelNode->m_model = model;
            m_models.push_back(model);
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

    m_rootNode = rootNode;
}