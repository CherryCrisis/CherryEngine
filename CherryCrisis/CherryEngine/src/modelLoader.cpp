#include "modelLoader.hpp"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/matrix4x4.h>

#include "resourceManager.hpp"

#include "mesh.hpp"
#include "modelBase.hpp"
#include "model.hpp"


namespace CCModelLoader
{
    ModelNode* ProcessDataRecursive(const aiNode* node, ModelNode* parentModelNode, const aiScene* scene,
        std::vector<std::shared_ptr<Model>>& models,
        ResourceManager* resourceManager, const char* filepath)
    {
        ModelNode* modelNode = new ModelNode();
        modelNode->m_parentNode = parentModelNode;

        std::shared_ptr<Model> model;
        if (scene->mRootNode != node)
        {
            aiVector3D trs[3];
            node->mTransformation.Decompose(trs[2], trs[1], trs[0]);

            model = resourceManager->AddResource<Model>(filepath, false, scene, node);
            models.push_back(model);

            for (int i = 0; i < 3; ++i)
                for (int j = 0; j < 3; ++j)
                    modelNode->m_baseTRS[i].data[j] = trs[i][j];
        }

        modelNode->m_model = model;

        const size_t nbChildren = (size_t)node->mNumChildren;

        for (size_t i = 0; i < nbChildren; ++i)
        {
            ModelNode* childModel =
                ProcessDataRecursive(node->mChildren[i], modelNode, scene, models, resourceManager, filepath);

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

        /*(filepath, aiProcess_MakeLeftHanded |
            aiProcess_FlipWindingOrder |
            aiProcess_FlipUVs |
            aiProcess_CalcTangentSpace |
            aiProcess_GenSmoothNormals |
            aiProcess_Triangulate |
            aiProcess_FixInfacingNormals |
            aiProcess_FindInvalidData |
            aiProcess_ValidateDataStructure);*/

        if (scene)
        {
            ProcessData(scene, models, rootModels, filepath);
        }
        else
        {
            //printf("Error parsing : '%s'\n", importer.GetErrorString());
            return;
        }
    }
}



