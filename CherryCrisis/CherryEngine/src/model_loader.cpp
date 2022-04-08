#include "pch.hpp"

#include "model_loader.hpp"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/matrix4x4.h>
#include <format>
#include <fstream>
#include <string>
#include <glad/gl.h>
#include <stb_image.h>

#include "render_manager.hpp"
#include "resource_manager.hpp"
#include "debug.hpp"


#include "mesh.hpp"
#include "model_base.hpp"
#include "model.hpp"

//TODO: Remove this change CCFunction to accept multiple same T
struct CompressedArgs
{
    unsigned char** stbiData;
    unsigned char** cacheData;
    CCModelLoader::TextureHeader* textureHeader;
    std::mutex* mutex;
    std::condition_variable* condition;
};

void CompressTextureWithOpenGL(CompressedArgs& args)
{
    //TODO: remove when instance create in engine constructor
    RenderManager::GetInstance(); //TO initialize openGL

    /*https://www.oldunreal.com/editing/s3tc/ARB_texture_compression.pdf*/

    GLuint tex;
    glGenTextures(1, &tex);

    glBindTexture(GL_TEXTURE_2D, tex);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_COMPRESSED_RGBA, args.textureHeader->width, args.textureHeader->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, *args.stbiData);

    int compressed;

    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_COMPRESSED, &compressed);

    /* if the compression has been successful */
    if (compressed == GL_TRUE)
    {
        glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_INTERNAL_FORMAT, &args.textureHeader->internalFormat);
        glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_COMPRESSED_IMAGE_SIZE, &args.textureHeader->compressedSize);

        *args.cacheData = (unsigned char*)malloc(args.textureHeader->compressedSize);
        glGetCompressedTexImage(GL_TEXTURE_2D, 0, *args.cacheData);
    }

    if (args.mutex)
    {
        std::unique_lock<std::mutex> uniqueMutex(*args.mutex);
        args.condition->notify_one();
    }
}

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

    void ImportMeshData(const aiScene* assimpScene, const aiNode* assimpNode, ImportModelUtils& model, const char* filepath)
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

    bool isCharacterToErase(unsigned char c) {
        return (c == ' ' || c == '/');
    }

    //Mutex and condition if the texture compression function is called on multithread
    void CacheTextureData(const char* texturePath, const unsigned char* cacheData, const TextureHeader& textureHeader)
    {
        Debug* debug = Debug::GetInstance();

        //TODO: change location
        std::string texturePathStr(texturePath);
        texturePathStr.erase(std::remove_if(texturePathStr.begin(), texturePathStr.end(), isCharacterToErase), texturePathStr.end());
        texturePathStr += ".ccfile";

        std::string allTexturePath("Assets/");
        allTexturePath += texturePathStr;

        FILE* file = nullptr;

        if (fopen_s(&file, allTexturePath.c_str(), "wb")) //w+b = write in binary mode
        {
            debug->AddLog(ELogType::ERROR, std::format("{} {}", "Failed to cache texture", allTexturePath).c_str());
            return;
        }

        fwrite(&textureHeader, sizeof(TextureHeader), 1, file);

        fwrite(&cacheData[0], textureHeader.compressedSize, 1, file);
        fclose(file);
    }

    void ImportTextureData(const char* texturePath, const aiTexture* assimpTexture)
    {
        unsigned int len;

        if (assimpTexture->mHeight)
            len = assimpTexture->mHeight * assimpTexture->mWidth * sizeof(assimpTexture->pcData);
        else
            len = assimpTexture->mWidth;

        int width;
        int height;

        unsigned char* stbiData = stbi_load_from_memory((unsigned char*)assimpTexture->pcData, len, &width, &height, 0, STBI_rgb_alpha);

        if (!stbiData)
        {
            Debug* debug = Debug::GetInstance();
            debug->AddLog(ELogType::ERROR, std::format("{}", "Failed to load texture").c_str());
            return;
        }

        unsigned char* cacheData = nullptr;
        TextureHeader textureHeader{.height = height, .width = width};

        CompressedArgs args{ &stbiData, &cacheData, &textureHeader, nullptr, nullptr};

        if (ThreadPool::GetInstance()->GetMainThreadID() != std::this_thread::get_id())
        {
            std::mutex mutex;
            std::condition_variable condition;

            std::unique_lock<std::mutex> uniqueMutex(mutex);

            args.mutex = &mutex;
            args.condition = &condition;

            //To compress texture with openGL
            auto function = CCFunction::BindFunctionUnsafe(&CompressTextureWithOpenGL, args);
            ThreadPool::GetInstance()->CreateTask(function, EChannelTask::MAINTHREAD);
            //Wait for the texture to compress
            condition.wait(uniqueMutex);
        }
        else
        {
            CompressTextureWithOpenGL(args);
        }

        stbi_image_free(stbiData);

        CacheTextureData(texturePath, cacheData, textureHeader);
    }

    void ImportTextureData(const char* texturePath)
    {
        stbi_set_flip_vertically_on_load(true);

        int width;
        int height;

        std::string allTexturePath("Assets/");
        allTexturePath += texturePath;

        unsigned char* stbiData = stbi_load(allTexturePath.c_str(), &width, &height, 0, STBI_rgb_alpha);

        if (!stbiData)
        {
            Debug* debug = Debug::GetInstance();
            debug->AddLog(ELogType::ERROR, std::format("{} {}", "Failed to load image", texturePath).c_str());
        }

        unsigned char* cacheData = nullptr;
        TextureHeader textureHeader{ .height = height, .width = width };

        CompressedArgs args{ &stbiData, &cacheData, &textureHeader, nullptr, nullptr };

        if (ThreadPool::GetInstance()->GetMainThreadID() != std::this_thread::get_id())
        {
            std::mutex mutex;
            std::condition_variable condition;

            std::unique_lock<std::mutex> uniqueMutex(mutex);

            args.mutex = &mutex;
            args.condition = &condition;

            //To compress texture with openGL
            auto function = CCFunction::BindFunctionUnsafe(&CompressTextureWithOpenGL, args);
            ThreadPool::GetInstance()->CreateTask(function, EChannelTask::MAINTHREAD);
            //Wait for the texture to compress
            condition.wait(uniqueMutex);
        }
        else
        {
            CompressTextureWithOpenGL(args);
        }

        CacheTextureData(texturePath, cacheData, textureHeader);

    }

    bool VerifIfTextureCacheExist(const char* texturePath)
    {
        std::string texturePathStr(texturePath);
        texturePathStr.erase(std::remove_if(texturePathStr.begin(), texturePathStr.end(), isCharacterToErase), texturePathStr.end());
        texturePathStr += ".ccfile";

        std::string allTexturePath("Assets/");
        allTexturePath += texturePathStr;

        FILE* file;
        if (fopen_s(&file, allTexturePath.c_str(), "r"))
        {
            return false;
        }
        else 
        {
            fclose(file);
            return true;
        }
    }

    void AddTextureDataToModel(ImportModelUtils& model, ETextureType textureType, const char* texturePath)
    {
        MaterialHeader* materialHeader = &model.modelHeader.m_materialHeader;

        ++materialHeader->m_texturesCount;

        model.m_texturesType.push_back((unsigned int)textureType);

        std::string texturePathStr(texturePath);
        texturePathStr.erase(std::remove_if(texturePathStr.begin(), texturePathStr.end(), isCharacterToErase), texturePathStr.end());
        //texturePathStr.erase(std::remove_if(texturePathStr.begin(), texturePathStr.end(), "'\'"), texturePathStr.end());
        texturePathStr += ".ccfile";

        const char* texturePathCstr = texturePathStr.c_str();

        size_t texturePathSize = std::strlen(texturePathCstr);

        model.m_texturesPathSize.push_back(static_cast<unsigned int>(texturePathSize));
        model.m_texturesPath.insert(model.m_texturesPath.end(), &texturePathCstr[0], &texturePathCstr[texturePathSize]);
        model.m_texturesPathCstr.push_back(texturePathCstr);
    }

    void ImportMaterialData(const aiScene* assimpScene, const aiNode* assimpNode, ImportModelUtils& model, const char* filepath)
    {
        if (assimpNode->mNumMeshes == 0)
        {
            model.modelHeader.m_materialHeader.m_hasMaterial = false;
            return;
        }

        model.modelHeader.m_materialHeader.m_hasMaterial = true;

        MaterialHeader* materialHeader = &model.modelHeader.m_materialHeader;

        size_t meshId = (size_t)assimpNode->mMeshes[0];
        
        const aiMesh* assimpMesh = assimpScene->mMeshes[meshId];
        const aiMaterial* assimpMaterial = assimpScene->mMaterials[assimpMesh->mMaterialIndex];

        aiString name = assimpMaterial->GetName();
        std::string materialPath = filepath + std::string("/") + std::string(name.C_Str());
        materialHeader->m_matarialId = std::hash<std::string>{}(materialPath);

        //Material Color
        aiColor3D color(0.f, 0.f, 0.f);
        if (AI_SUCCESS == assimpMaterial->Get(AI_MATKEY_COLOR_DIFFUSE, color))
            materialHeader->m_albedo = Vector3(color.r, color.g, color.b);

        //Material Texture
        {
            aiString texturePath;
            if (assimpMaterial->GetTexture(aiTextureType_AMBIENT, 0, &texturePath) == AI_SUCCESS)
            {
                if (!VerifIfTextureCacheExist(texturePath.C_Str()))
                    ImportTextureData(texturePath.C_Str());

                AddTextureDataToModel(model, ETextureType::AMBIENT, texturePath.C_Str());
            }

            assimpMaterial->Get(AI_MATKEY_TEXTURE(aiTextureType_DIFFUSE, 0), texturePath);
            if (auto texture = assimpScene->GetEmbeddedTexture(texturePath.C_Str()))
            {
                if (!VerifIfTextureCacheExist(texturePath.C_Str()))
                    ImportTextureData(texturePath.C_Str(), texture);

                AddTextureDataToModel(model, ETextureType::ALBEDO, texturePath.C_Str());
            }
            else
            {
                if (!VerifIfTextureCacheExist(texturePath.C_Str()))
                    ImportTextureData(texturePath.C_Str());

                AddTextureDataToModel(model, ETextureType::ALBEDO, texturePath.C_Str());
            }
        }
    }

    void ImportModelDataRecursive(const aiNode* node, int modelParentIndex, unsigned int& index,
        const aiScene* scene, std::vector<ImportModelUtils>& models, const char* filepath)
    {
        ImportModelUtils model {};

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

        ImportMeshData(scene, node, model, filepath);
        ImportMaterialData(scene, node, model, filepath);

        model.modelHeader.m_childrenCount = node->mNumChildren;

        models.push_back(model);

        int indexBuff = index;
        ++index;

        std::vector<unsigned int> childrenIndexs;
        childrenIndexs.reserve(model.modelHeader.m_childrenCount * sizeof(unsigned int));
        for (unsigned int i = 0; i < node->mNumChildren; ++i)
        {
            childrenIndexs.push_back(index);
            ImportModelDataRecursive(node->mChildren[i], model.modelHeader.m_index, index, scene, models, filepath);
        }

        if (node->mNumChildren)
        {
            models[indexBuff].m_childrenIndices.swap(childrenIndexs);
        }
    }


    void ImportModelData(const aiScene* scene, std::vector<ImportModelUtils>& models, const char* filepath)
    {
        unsigned int currentIndex = 0;
        ImportModelDataRecursive(scene->mRootNode, -1, currentIndex, scene, models, filepath);
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

        if (scene)
        {
            ImportModelData(scene, models, filepath);

            FILE* file = nullptr;

            std::string fileIDStr = filepath;
            fileIDStr += ".ccfile";

            if (fopen_s(&file, fileIDStr.c_str(), "wb"))
            {
                debug->AddLog(ELogType::ERROR, std::format("Failed to open/create file : {}", fileIDStr).c_str());
                return;
            }

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

                if (model.modelHeader.m_materialHeader.m_hasMaterial)
                {
                    fwrite(&model.m_texturesPathSize[0], model.m_texturesPathSize.size() * sizeof(unsigned int), 1, file);
                    fwrite(&model.m_texturesType[0], model.m_texturesType.size() * sizeof(unsigned int), 1, file);
                    fwrite(&model.m_texturesPath[0], model.m_texturesPath.size(), 1, file);
                }
            }

            fclose(file);
        }
        else
        {
            debug->AddLog(ELogType::ERROR, std::format("Error parsing : {}", importer.GetErrorString()).c_str());
            return;
        }
    }

    void ImportTexture(const char* texturePath, unsigned char** textureData, TextureHeader& textureHeader)
    {
        stbi_set_flip_vertically_on_load(true);

        std::string allTexturePath("Assets/");
        allTexturePath += texturePath;

        unsigned char* stbiData = stbi_load(allTexturePath.c_str(), &textureHeader.width, &textureHeader.height, 0, STBI_rgb_alpha);

        if (!stbiData)
        {
            Debug* debug = Debug::GetInstance();
            debug->AddLog(ELogType::ERROR, std::format("{} {}", "Failed to load image", texturePath).c_str());
        }

        CompressedArgs args{ &stbiData, textureData, &textureHeader, nullptr, nullptr };

        if (ThreadPool::GetInstance()->GetMainThreadID() != std::this_thread::get_id())
        {
            std::mutex mutex;
            std::condition_variable condition;

            std::unique_lock<std::mutex> uniqueMutex(mutex);

            args.mutex = &mutex;
            args.condition = &condition;

            //To compress texture with openGL
            auto function = CCFunction::BindFunctionUnsafe(&CompressTextureWithOpenGL, args);
            ThreadPool::GetInstance()->CreateTask(function, EChannelTask::MAINTHREAD);
            //Wait for the texture to compress
            condition.wait(uniqueMutex);
        }
        else
        {
            CompressTextureWithOpenGL(args);
        }

        CacheTextureData(texturePath, *textureData, textureHeader);
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



