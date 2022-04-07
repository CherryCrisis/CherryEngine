#include "pch.hpp"

#include "model_loader.hpp"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/matrix4x4.h>
#include <format>
#include <fstream>
#include <string>

#include "resource_manager.hpp"
#include "debug.hpp"


#include "mesh.hpp"
#include "model_base.hpp"
#include "model.hpp"


namespace CCModelLoader
{
    /*
    #pragma region Load

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
            std::string modelName = filepath + std::string("/") + std::to_string(models.size());
            model = resourceManager->AddResource<Model>(modelName.c_str(), true, scene, node, filepath);
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
                                                           aiProcess_JoinIdenticalVertices |
                                                           aiProcess_SortByPType |
                                                           aiProcess_GenNormals |
                                                           aiProcess_GenUVCoords |
                                                           aiProcess_FlipUVs |
                                                           aiProcess_CalcTangentSpace);

        if (scene)
        {
            ProcessData(scene, models, rootModels, filepath);
        }
        else
        {
            Debug::GetInstance()->AddLog(ELogType::ERROR, std::format("Error parsing : {}", importer.GetErrorString()).c_str());
            return;
        }
    }
#pragma endregion
*/

    void ImportMeshData(const aiScene* assimpScene, const aiNode* assimpNode, ImportModelUtils& model, const char* filepath, FILE* file)
    {
        /* model->m_mesh = resourceManager->AddResource<Mesh>(meshPath.c_str(), true, assimpMesh); */

        if (assimpNode->mNumMeshes == 0)
        {
            model.modelHeader.m_hasMesh = false;
            return;
        }

        model.modelHeader.m_hasMesh = true;


        size_t meshId = (size_t)assimpNode->mMeshes[0];
        const aiMesh* assimpMesh = assimpScene->mMeshes[meshId];

        std::string meshPath = filepath + std::string("/") + std::string(assimpMesh->mName.C_Str());
        model.modelHeader.m_meshId = std::hash<std::string>{}(meshPath);

        std::vector<Vertex> vertices;

        size_t verticesSize = assimpMesh->mNumVertices * sizeof(Vertex);
        vertices.reserve(verticesSize);

        for (size_t v = 0u; v < assimpMesh->mNumVertices; v++)
        {
            Vertex& vertex = vertices.emplace_back();
            for (int i = 0; i < 3; ++i)
            {
                vertex.position.data[i] = assimpMesh->mVertices[v][i];
                vertex.normal.data[i] = assimpMesh->mNormals[v][i];

                if (assimpMesh->HasTangentsAndBitangents())
                {
                    vertex.tangent.data[i] = assimpMesh->mTangents[v][i];
                    vertex.bitangent.data[i] = assimpMesh->mBitangents[v][i];
                }
            }

            if (assimpMesh->HasTextureCoords(0)) {
                vertex.uv.data[0] = assimpMesh->mTextureCoords[0][v].x;
                vertex.uv.data[1] = assimpMesh->mTextureCoords[0][v].y;
            }
            else
            {
                vertex.uv = { 0.f };
            }
        }

        std::vector<unsigned int> indices;
        size_t indicesSize = (size_t)(assimpMesh->mNumFaces) * 3u;
        indices.reserve(indicesSize);

        for (size_t f = 0; f < assimpMesh->mNumFaces; f++)
        {
            for (int i = 0; i < 3; ++i)
            {
                indices.push_back(assimpMesh->mFaces[f].mIndices[i]);
            }
        }

        model.modelHeader.m_verticesCount = vertices.size();
        model.modelHeader.m_indicesCount = indices.size();
        model.m_vertices.swap(vertices);
        model.m_indices.swap(indices);
    }

    //void ImportMaterialData(const aiScene* assimpScene, const aiNode* assimpNode, ModelHeader::MaterialHeader& materialHeader, FILE* file)
    //{
    //
    //}

    void ImportModelDataRecursive(const aiNode* node, int modelParentIndex, unsigned int& index,
        const aiScene* scene, std::vector<ImportModelUtils>& models, const char* filepath, FILE* file)
    {
        ImportModelUtils model;

        std::string modelName = filepath + std::string("/") + std::to_string(index);
        model.modelHeader.m_modelId = std::hash<std::string>{}(modelName);
        model.modelHeader.m_index = index;
        model.modelHeader.m_parentIndex = modelParentIndex;

        aiVector3D trs[3];
        node->mTransformation.Decompose(trs[2], trs[1], trs[0]);

        for (int i = 0; i < 3; ++i)
        {
            for (int comp = 0; comp < 3; ++comp)
            {
                model.modelHeader.m_trs[i] = trs[i][comp];
            }
        }

        ImportMeshData(scene, node, model, filepath, file);

        model.modelHeader.m_childrenCount = node->mNumChildren;

        models.push_back(model);

        int indexBuff = index;
        ++index;

        std::vector<unsigned int> childrenIndexs;
        childrenIndexs.reserve(model.modelHeader.m_childrenCount * sizeof(unsigned int));
        for (unsigned int i = 0; i < node->mNumChildren; ++i)
        {
            childrenIndexs.push_back(index);
            ImportModelDataRecursive(node->mChildren[i], model.modelHeader.m_index, index, scene, models, filepath, file);
        }

        if (node->mNumChildren)
        {
            models[indexBuff].m_childrenIndices.swap(childrenIndexs);
        }
    }

    template<class T>
    void InsertDataToVector(std::vector<unsigned char>& vector, const T& value)
    {
        vector.insert(vector.end(), &((unsigned char*)&value)[0], &((unsigned char*)&value)[sizeof(T)]);
    }

    template<class T>
    void InsertDataToVector(std::vector<unsigned char>& vector, const T& value, const size_t size)
    {
        vector.insert(vector.end(), &((unsigned char*)&value)[0], &((unsigned char*)&value)[size]);
    }

    void ImportModelData(const aiScene* scene, std::vector<ImportModelUtils>& models, const char* filepath, FILE* file)
    {
        unsigned int currentIndex = 0;
        ImportModelDataRecursive(scene->mRootNode, -1, currentIndex, scene, models, filepath, file);
        unsigned int modelHeaderCount = currentIndex;

        size_t modelSize = models.size();
        fwrite(&modelSize, sizeof(size_t), 1, file);

        for (ImportModelUtils& model : models)
        {
            fwrite(&model.modelHeader, sizeof(ModelHeader), 1, file);

            if (model.modelHeader.m_childrenCount)
                fwrite(&model.m_childrenIndices[0], model.m_childrenIndices.size() * sizeof(unsigned int), 1, file);

            if (model.modelHeader.m_hasMesh)
            {
                fwrite(&model.m_vertices[0], model.m_vertices.size() * sizeof(Vertex), 1, file);
                fwrite(&model.m_indices[0], model.m_indices.size() * sizeof(unsigned int), 1, file);
            }
        }
    }

    void ImportModel(const char* filepath, std::vector<ImportModelUtils>& models)
    {
        Debug* debug = Debug::GetInstance();

        Assimp::Importer importer = Assimp::Importer();

        const aiScene* scene = importer.ReadFile(filepath, aiProcess_Triangulate |
            aiProcess_JoinIdenticalVertices |
            aiProcess_SortByPType |
            aiProcess_GenNormals |
            aiProcess_GenUVCoords |
            aiProcess_FlipUVs |
            aiProcess_CalcTangentSpace);

        FILE* file = nullptr;

        std::string fileIDStr = filepath;
        fileIDStr += ".ccfile";

        if (scene)
        {
            if (fopen_s(&file, fileIDStr.c_str(), "wb"))
            {
                debug->AddLog(ELogType::ERROR, std::format("Failed to open/create file : {}", fileIDStr).c_str());
                return;
            }

            ImportModelData(scene, models, filepath, file);

            fclose(file);
        }
        else
        {
            debug->AddLog(ELogType::ERROR, std::format("Error parsing : {}", importer.GetErrorString()).c_str());
            return;
        }
    }
    /*
    #pragma region Reload

    ModelNode* ReloadProcessDataRecursive(const aiNode* node, ModelNode* parentModelNode, const aiScene* scene,
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
            std::string modelName = filepath + std::string("/") + std::to_string(models.size());
            //model = resourceManager->AddResource<Model>(modelName.c_str(), true, scene, node, filepath);
            resourceManager->Reload<Model>(modelName.c_str(), scene, node);
            //models.push_back(model);
        }

        modelNode->m_model = model;

        const size_t nbChildren = (size_t)node->mNumChildren;

        for (size_t i = 0; i < nbChildren; ++i)
        {
            ModelNode* childModel = ReloadProcessDataRecursive(node->mChildren[i], modelNode, scene, models, resourceManager, filepath);

            //modelNode->m_childrenNode.push_back(childModel);
        }

        return modelNode;
    }

    void ReloadProcessData(const aiScene* scene,
        std::vector<std::shared_ptr<Model>>& models, ModelNode** rootModelNode, const char* filepath)
    {
        ResourceManager* resourceManager = ResourceManager::GetInstance();

        //models.reserve((size_t)scene->mNumMeshes);

        *rootModelNode = ReloadProcessDataRecursive(scene->mRootNode, nullptr, scene, models, resourceManager, filepath);
    }

    void ReloadModel(const char* filepath, ModelNode** rootModels, std::vector<std::shared_ptr<Model>>& models)
    {
        Assimp::Importer importer = Assimp::Importer();

        const aiScene* scene = importer.ReadFile(filepath, aiProcess_Triangulate |
            aiProcess_GenSmoothNormals |
            aiProcess_FlipUVs |
            aiProcess_JoinIdenticalVertices |
            aiProcess_CalcTangentSpace);

        if (scene)
        {
            ReloadProcessData(scene, models, rootModels, filepath);
        }
        else
        {
            printf("Error parsing : '%s'\n", importer.GetErrorString());
            return;
        }
    }

    #pragma endregion
    */
}



