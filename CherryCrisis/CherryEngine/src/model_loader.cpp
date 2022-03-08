#include "pch.hpp"

#include "model_loader.hpp"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/matrix4x4.h>

#include "resource_manager.hpp"

#include "mesh.hpp"
#include "model_base.hpp"
#include "model.hpp"


//namespace CCModelLoader
//{
    ModelNode* ProcessDataRecursive(const aiNode* node, ModelNode* parentModelNode, const aiScene* scene,
        std::vector<std::shared_ptr<Model>>& models,
        ResourceManager* resourceManager, const char* filepath)
    {
        ModelNode* modelNode = new ModelNode();
        modelNode->m_parentNode = parentModelNode;

        aiVector3D trs[3];
        node->mTransformation.Decompose(trs[2], trs[1], trs[0]);

        for (int i = 0; i < 3; ++i)
        {
            for (int comp = 0; comp < 3; ++comp)
            {
                modelNode->m_baseTRS[i].data[comp] = trs[i][comp];
            }
        }

        std::shared_ptr<Model> model;
        if (scene->mRootNode != node)
        {
            model = resourceManager->AddResource<Model>(filepath, false, scene, node);
            models.push_back(model);
        }

        modelNode->m_model = model;

        const size_t nbChildren = (size_t)node->mNumChildren;

        for (size_t i = 0; i < nbChildren; ++i)
        {
            ModelNode* childModel = ProcessDataRecursive(node->mChildren[i], modelNode, scene, models, resourceManager, filepath);

            modelNode->m_childrenNode.push_back(childModel);
        }

        return modelNode;
    }

    void ProcessData(const aiScene* scene,
        std::vector<std::shared_ptr<Model>>& models, ModelNode** rootModelNode, const char* filepath)
    {
        ResourceManager* resourceManager = ResourceManager::GetInstance();

        models.reserve((size_t)scene->mNumMeshes);

        *rootModelNode = ProcessDataRecursive(scene->mRootNode, nullptr, scene, models, resourceManager, filepath);
    }

    void LoadModel(const char* filepath, ModelNode** rootModels, std::vector<std::shared_ptr<Model>>& models)
    {

        Assimp::Importer importer = Assimp::Importer();

        const aiScene* scene = importer.ReadFile(filepath, aiProcess_Triangulate |
                                                           aiProcess_GenSmoothNormals |
                                                           aiProcess_FlipUVs |
                                                           aiProcess_JoinIdenticalVertices |
                                                           aiProcess_CalcTangentSpace);

        if (scene)
        {
            std::cout << "Model load processing" << std::endl;
            ProcessData(scene, models, rootModels, filepath);
        }
        else
        {
            printf("Error parsing : '%s'\n", importer.GetErrorString());
            return;
        }

        std::cout << "Parsing ended" << std::endl;
    }
//}



