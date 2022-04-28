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
#include <stb_image_write.h>
#include <nvtt/nvtt.h>

#include "render_manager.hpp"
#include "resource_manager.hpp"
#include "debug.hpp"

#include "mesh.hpp"
#include "model_base.hpp"
#include "model.hpp"
#include "texture.hpp"
#include "utils.hpp"

namespace CCImporter
{
    #pragma region Mesh

    void ImportMeshData(const aiScene* assimpScene, const aiNode* assimpNode, int meshIndex, ImportModelUtils& model, const std::filesystem::path& filepath)
    {
        if (assimpNode->mNumMeshes == 0)
        {
            model.modelHeader.m_meshHeader.m_hasMesh = false;
            return;
        }

        model.modelHeader.m_meshHeader.m_hasMesh = true;

        size_t meshId = (size_t)assimpNode->mMeshes[meshIndex];
        const aiMesh* assimpMesh = assimpScene->mMeshes[meshId];

        model.m_meshName = filepath.string();
        model.m_meshName += std::string("/");
        model.m_meshName += std::string(assimpMesh->mName.C_Str());

        model.modelHeader.m_meshHeader.m_meshNameSize = model.m_meshName.size();

        std::vector<Vertex> vertices;

        size_t verticesSize = assimpMesh->mNumVertices * sizeof(Vertex);
        vertices.reserve(verticesSize);

        for (size_t v = 0u; v < assimpMesh->mNumVertices; v++)
        {
            Vertex& vertex = vertices.emplace_back();
            for (int i = 0; i < 3; ++i)
            {
                vertex.position.data[i] = assimpMesh->mVertices[v][i];

                if (assimpMesh->HasNormals())
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

        model.modelHeader.m_meshHeader.m_verticesCount = (unsigned int)vertices.size();
        model.modelHeader.m_meshHeader.m_indicesCount = (unsigned int)indices.size();
        model.m_vertices.swap(vertices);
        model.m_indices.swap(indices);
    }
    
    #pragma endregion

    #pragma region Texture

    struct OuputHandler : nvtt::OutputHandler
    {
        std::vector<char> m_data;

        virtual void beginImage(int size, int width, int height, int depth, int face, int miplevel) override
        {
        }

        bool writeData(const void* data, int size) override
        {
            m_data.insert(m_data.end(), &((unsigned char*)data)[0], &((unsigned char*)data)[0] + size);
            return true;
        }

        void endImage() override {};
    };

    void CacheTextureData(const std::string& filepath, const unsigned char* cacheData, TextureHeader& textureHeader)
    {
        FILE* file = nullptr;

        if (!std::filesystem::exists(CCImporter::cacheDirectory))
            std::filesystem::create_directory(CCImporter::cacheDirectory);

        if (fopen_s(&file, filepath.c_str(), "wb")) //w+b = write in binary mode
        {
            Debug::GetInstance()->AddLog(ELogType::ERROR, std::format("{} {}", "Failed to cache texture", filepath).c_str());
            return;
        }

        fwrite(&textureHeader, sizeof(TextureHeader), 1, file);
        fwrite(&cacheData[0], textureHeader.size, 1, file);

        fclose(file);
    }

    void WriteTexturePng(const std::filesystem::path& filepath, const unsigned char* cacheData, const TextureHeader& textureHeader)
    {
        if (!cacheData)
            return;

        stbi_flip_vertically_on_write(true);

        std::string filepathStr(filepath.string());
        if (!stbi_write_png(filepathStr.c_str(), textureHeader.width, textureHeader.height, 4, cacheData, textureHeader.width * 4))
        {
            Debug::GetInstance()->AddLog(ELogType::ERROR, std::format("Failed to write png file : {}", filepathStr).c_str());
        }
    }

    void SetTextureFormat(nvtt::CompressionOptions& compressionOptions, TextureHeader& textureHeader)
    {
        switch (textureHeader.internalFormat)
        {
        case ETextureFormat::RGB:
            compressionOptions.setFormat(nvtt::Format_RGB);
            compressionOptions.setPixelFormat(8, 8, 8, 0);
            textureHeader.blockSize = 8;
            break;
        case ETextureFormat::RGBA:
            compressionOptions.setFormat(nvtt::Format_RGBA);
            textureHeader.blockSize = 8;
            break;
        case ETextureFormat::DXT1:
            compressionOptions.setFormat(nvtt::Format_BC1);
            textureHeader.blockSize = 8;
            break;
        case ETextureFormat::DXT1a:
            compressionOptions.setFormat(nvtt::Format_BC1a);
            textureHeader.blockSize = 8;
            break;
        case ETextureFormat::DXT3:
            compressionOptions.setFormat(nvtt::Format_BC3);
            textureHeader.blockSize = 16;
            break;
        case ETextureFormat::DXT5:
            compressionOptions.setFormat(nvtt::Format_BC5);
            textureHeader.blockSize = 16;
            break;
        case ETextureFormat::DXT6:
            compressionOptions.setFormat(nvtt::Format_BC6S);
            textureHeader.blockSize = 16;
            break;
        case ETextureFormat::DXT7:
            compressionOptions.setFormat(nvtt::Format_BC7);
            textureHeader.blockSize = 16;
            break;
        }
    }
    
    void CompressTexture(nvtt::Context& context, nvtt::Surface& image, std::string& texturePath, TextureHeader& textureHeader, unsigned char** textureData)
    {
        nvtt::OutputOptions outputOptions;
        outputOptions.setFileName(texturePath.c_str());
        nvtt::CompressionOptions compressionOptions;
        SetTextureFormat(compressionOptions, textureHeader);

        if (textureHeader.internalFormat == ETextureFormat::DXT5)
            image.setNormalMap(true);

        textureHeader.mipmapsLevel = image.countMipmaps();

        OuputHandler outputHandler;
        outputOptions.setOutputHandler(((nvtt::OutputHandler*)&outputHandler));

        /*https://github.com/nvpro-samples/nvtt_samples/blob/main/mipmap/main.cpp*/

        //context.outputHeader(image, textureHeader.mipmapsLevel, compressionOptions, outputOptions);
        for (int i = 0; textureHeader.mipmapsLevel; ++i)
        {
            context.compress(image, 0, i, compressionOptions, outputOptions);

            if (i == textureHeader.mipmapsLevel - 1) break;

            // Prepare the next mip:
            // 
            // Convert to linear premultiplied alpha. Note that toLinearFromSrgb()
            // will clamp HDR images; consider e.g. toLinear(2.2f) instead.
            image.toLinearFromSrgb();
            image.premultiplyAlpha();

            // Resize the image to the next mipmap size.
            // NVTT has several mipmapping filters; Box is the lowest-quality, but
            // also the fastest to use.
            image.buildNextMipmap(nvtt::MipmapFilter_Box);
            // For general image resizing. use image.resize().

            // Convert back to unpremultiplied sRGB.
            image.demultiplyAlpha();
            image.toSrgb();
        }

        textureHeader.size = (int)outputHandler.m_data.size();

        *textureData = new unsigned char[textureHeader.size];
        std::move(outputHandler.m_data.begin(), outputHandler.m_data.end(), *textureData);
    }

    void ImportTextureData(const std::filesystem::path& filepath, const aiTexture* assimpTexture)
    {
        unsigned int len;

        if (assimpTexture->mHeight)
            len = assimpTexture->mHeight * assimpTexture->mWidth * sizeof(assimpTexture->pcData);
        else
            len = assimpTexture->mWidth;

        TextureHeader textureHeader;

        unsigned char* stbiData = stbi_load_from_memory((unsigned char*)assimpTexture->pcData, len, &textureHeader.width, &textureHeader.height, 0, STBI_rgb_alpha);

        if (!stbiData)
        {
            Debug* debug = Debug::GetInstance();
            debug->AddLog(ELogType::ERROR, std::format("{}", "Failed to load texture").c_str());
            return;
        }

        textureHeader.size = textureHeader.height * textureHeader.width * 4;

        WriteTexturePng(filepath, stbiData, textureHeader);

        nvtt::Context context(false);

        nvtt::Surface image;
        image.load(filepath.string().c_str());
        image.setImage2D(nvtt::Format_RGBA, textureHeader.width, textureHeader.height, stbiData);

        textureHeader.internalFormat = ETextureFormat::RGBA;

        std::string fullFilepath(CCImporter::cacheDirectory);
        fullFilepath += filepath.filename().string();
        fullFilepath += CCImporter::cacheExtension;

        unsigned char* textureData;

        CompressTexture(context, image, fullFilepath, textureHeader, &textureData);
        CacheTextureData(fullFilepath, textureData, textureHeader);

        delete textureData;
        stbi_image_free(stbiData);
    }

    void ImportTextureData(const std::filesystem::path& filepath)
    {
        unsigned char* data;
        TextureHeader textureHeader;
        ImportTexture(filepath, &data, textureHeader, true, ETextureFormat::RGBA);
        delete data;
    }

    bool VerifIfTextureCacheExist(const char* textureFilename)
    {
        std::string texturePath(CCImporter::cacheDirectory);
        texturePath += textureFilename;
        texturePath += CCImporter::cacheExtension;

        FILE* file;
        if (fopen_s(&file, texturePath.c_str(), "r"))
        {
            return false;
        }
        else 
        {
            fclose(file);
            return true;
        }
    }

    void AddTextureDataToModel(ImportModelUtils& model, ETextureType textureType, const std::filesystem::path& filepath)
    {
        MaterialHeader* materialHeader = &model.modelHeader.m_materialHeader;

        ++materialHeader->m_texturesCount;

        model.m_texturesType.push_back((unsigned int)textureType);

        std::string filepathStr(filepath.string());
        size_t texturePathSize = filepathStr.size();

        model.m_texturesPathSize.push_back(static_cast<unsigned int>(texturePathSize));
        model.m_texturesPath.insert(model.m_texturesPath.end(), &filepathStr[0], &filepathStr[texturePathSize]);
        model.m_texturesPathCstr.push_back(filepathStr);
    }

    void ImportTexture(const std::filesystem::path& filepath, unsigned char** textureData, TextureHeader& textureHeader, bool flipTexture, ETextureFormat textureFormat)
    {
        if (!textureData)
            return;

        nvtt::Context context;

        nvtt::Surface image;
        image.load(filepath.string().c_str());

        if (flipTexture)
            image.flipY();

        //textureHeader.internalFormat = textureFormat;
        textureHeader.internalFormat = textureFormat;
        textureHeader.width = image.width();
        textureHeader.height = image.height();

        std::string fullFilepath(CCImporter::cacheDirectory);
        fullFilepath += filepath.filename().string();
        fullFilepath += CCImporter::cacheExtension;

        //std::string texturePathStr(filepath.filename().string());
        CompressTexture(context, image, fullFilepath, textureHeader, textureData);

        CacheTextureData(fullFilepath, *textureData, textureHeader);
    }
    #pragma endregion

    #pragma region Material

    void GetFBXTexturePath(std::filesystem::path& textureFilepath, const std::filesystem::path& filepath, const char* texturefbxFilename)
    {
        std::filesystem::path texturefbxFilepath(texturefbxFilename);

        std::string textureFilepathStr(filepath.parent_path().string());
        textureFilepathStr += "/";
        textureFilepathStr += texturefbxFilepath.filename().string();

        size_t lastindex = textureFilepathStr.find_last_of(".");
        textureFilepathStr.erase(lastindex, textureFilepathStr.size());

        textureFilepathStr += ".png";

        textureFilepath = textureFilepathStr;
    }

    void ImportMaterialData(const aiScene* assimpScene, const aiNode* assimpNode, int meshIndex, ImportModelUtils& model, const std::filesystem::path& filepath)
    {
        if (assimpNode->mNumMeshes == 0)
        {
            model.modelHeader.m_materialHeader.m_hasMaterial = false;
            return;
        }

        model.modelHeader.m_materialHeader.m_hasMaterial = true;

        MaterialHeader* materialHeader = &model.modelHeader.m_materialHeader;

        size_t meshId = (size_t)assimpNode->mMeshes[meshIndex];
        
        const aiMesh* assimpMesh = assimpScene->mMeshes[meshId];
        const aiMaterial* assimpMaterial = assimpScene->mMaterials[assimpMesh->mMaterialIndex];

        aiString name(assimpMaterial->GetName());

        if (name.length == 0)
            name = std::format("mat_{}", assimpMesh->mMaterialIndex).c_str();

        model.m_materialName = filepath.string();
        model.m_materialName += std::string("/");
        model.m_materialName += std::string(name.C_Str());

        materialHeader->m_materialNameSize = model.m_materialName.size();

        float matValue = 0.f;
        if (AI_SUCCESS == assimpMaterial->Get(AI_MATKEY_SHININESS, matValue))
            materialHeader->m_shininess = matValue;

        if (AI_SUCCESS == assimpMaterial->Get(AI_MATKEY_SPECULAR_FACTOR, matValue))
            materialHeader->m_specularFactor = matValue;

        if (AI_SUCCESS == assimpMaterial->Get(AI_MATKEY_METALLIC_FACTOR, matValue))
            materialHeader->m_metallicFactor = matValue;

        if (AI_SUCCESS == assimpMaterial->Get(AI_MATKEY_ROUGHNESS_FACTOR, matValue))
            materialHeader->m_roughnessFactor = matValue;

        if (AI_SUCCESS == assimpMaterial->Get(AI_MATKEY_CLEARCOAT_FACTOR, matValue))
            materialHeader->m_clearCoatFactor = matValue;

        if (AI_SUCCESS == assimpMaterial->Get(AI_MATKEY_CLEARCOAT_ROUGHNESS_FACTOR, matValue))
            materialHeader->m_clearCoatRoughnessFactor = matValue;

        //Material Color
        aiColor3D color(0.f, 0.f, 0.f);
        if (AI_SUCCESS == assimpMaterial->Get(AI_MATKEY_COLOR_AMBIENT, color))
        {
            materialHeader->m_ambient = Vector3(color.r, color.g, color.b);
            materialHeader->m_ao = color.r;
        }

        if (AI_SUCCESS == assimpMaterial->Get(AI_MATKEY_COLOR_DIFFUSE, color))
            materialHeader->m_diffuse = Vector3(color.r, color.g, color.b);

        if (AI_SUCCESS == assimpMaterial->Get(AI_MATKEY_COLOR_SPECULAR, color))
            materialHeader->m_specular = Vector3(color.r, color.g, color.b);

        if (AI_SUCCESS == assimpMaterial->Get(AI_MATKEY_COLOR_EMISSIVE, color))
            materialHeader->m_emissive = Vector3(color.r, color.g, color.b);

        //Material Texture
        {
            aiString textureFilename;
            if (assimpMaterial->GetTexture(aiTextureType_AMBIENT, 0, &textureFilename) == AI_SUCCESS)
            {
                if (auto texture = assimpScene->GetEmbeddedTexture(textureFilename.C_Str()))
                {
                    std::filesystem::path textureFilepath;
                    GetFBXTexturePath(textureFilepath, filepath, textureFilename.C_Str());

                    if (!VerifIfTextureCacheExist(textureFilepath.filename().string().c_str()))
                        ImportTextureData(textureFilepath, texture);

                    AddTextureDataToModel(model, ETextureType::AMBIENT, textureFilepath);
                }
                else
                {
                    std::filesystem::path textureFilepath(filepath.parent_path());
                    textureFilepath += "/";
                    textureFilepath += textureFilename.C_Str();

                    if (!VerifIfTextureCacheExist(textureFilepath.filename().string().c_str()))
                        ImportTextureData(textureFilepath);

                    AddTextureDataToModel(model, ETextureType::AMBIENT, textureFilepath);
                }
            }

            
            if (assimpMaterial->Get(AI_MATKEY_TEXTURE(aiTextureType_HEIGHT, 0), textureFilename) == AI_SUCCESS)
            {

                if (auto texture = assimpScene->GetEmbeddedTexture(textureFilename.C_Str()))
                {
                    std::filesystem::path textureFilepath;
                    GetFBXTexturePath(textureFilepath, filepath, textureFilename.C_Str());

                    if (!VerifIfTextureCacheExist(textureFilepath.filename().string().c_str()))
                        ImportTextureData(textureFilepath, texture);

                    AddTextureDataToModel(model, ETextureType::NORMAL_MAP, textureFilepath);
                }
                else
                {
                    std::filesystem::path textureFilepath(filepath.parent_path());
                    textureFilepath += "/";
                    textureFilepath += textureFilename.C_Str();

                    if (!VerifIfTextureCacheExist(textureFilepath.filename().string().c_str()))
                        ImportTextureData(textureFilepath);

                    AddTextureDataToModel(model, ETextureType::NORMAL_MAP, textureFilepath);
                }
            }

            if (assimpMaterial->Get(AI_MATKEY_TEXTURE(aiTextureType_DIFFUSE, 0), textureFilename) == AI_SUCCESS)
            {

                if (auto texture = assimpScene->GetEmbeddedTexture(textureFilename.C_Str()))
                {
                    std::filesystem::path textureFilepath;
                    GetFBXTexturePath(textureFilepath, filepath, textureFilename.C_Str());

                    if (!VerifIfTextureCacheExist(textureFilepath.filename().string().c_str()))
                        ImportTextureData(textureFilepath, texture);

                    AddTextureDataToModel(model, ETextureType::ALBEDO, textureFilepath);
                }
                else
                {
                    std::filesystem::path textureFilepath(filepath.parent_path());
                    textureFilepath += "/";
                    textureFilepath += textureFilename.C_Str();

                    if (!VerifIfTextureCacheExist(textureFilepath.filename().string().c_str()))
                        ImportTextureData(textureFilepath);

                    AddTextureDataToModel(model, ETextureType::ALBEDO, textureFilepath);
                }
            }

            if (assimpMaterial->Get(AI_MATKEY_TEXTURE(aiTextureType_METALNESS, 0), textureFilename) == AI_SUCCESS)
            {

                if (auto texture = assimpScene->GetEmbeddedTexture(textureFilename.C_Str()))
                {
                    std::filesystem::path textureFilepath;
                    GetFBXTexturePath(textureFilepath, filepath, textureFilename.C_Str());

                    if (!VerifIfTextureCacheExist(textureFilepath.filename().string().c_str()))
                        ImportTextureData(textureFilepath, texture);

                    AddTextureDataToModel(model, ETextureType::METALLIC, textureFilepath);
                }
                else
                {
                    std::filesystem::path textureFilepath(filepath.parent_path());
                    textureFilepath += "/";
                    textureFilepath += textureFilename.C_Str();

                    if (!VerifIfTextureCacheExist(textureFilepath.filename().string().c_str()))
                        ImportTextureData(textureFilepath);

                    AddTextureDataToModel(model, ETextureType::METALLIC, textureFilepath);
                }
            }

            if (assimpMaterial->Get(AI_MATKEY_TEXTURE(aiTextureType_DIFFUSE_ROUGHNESS, 0), textureFilename) == AI_SUCCESS)
            {

                if (auto texture = assimpScene->GetEmbeddedTexture(textureFilename.C_Str()))
                {
                    std::filesystem::path textureFilepath;
                    GetFBXTexturePath(textureFilepath, filepath, textureFilename.C_Str());

                    if (!VerifIfTextureCacheExist(textureFilepath.filename().string().c_str()))
                        ImportTextureData(textureFilepath, texture);

                    AddTextureDataToModel(model, ETextureType::ROUGHNESS, textureFilepath);
                }
                else
                {
                    std::filesystem::path textureFilepath(filepath.parent_path());
                    textureFilepath += "/";
                    textureFilepath += textureFilename.C_Str();

                    if (!VerifIfTextureCacheExist(textureFilepath.filename().string().c_str()))
                        ImportTextureData(textureFilepath);

                    AddTextureDataToModel(model, ETextureType::ROUGHNESS, textureFilepath);
                }
            }

            if (assimpMaterial->Get(AI_MATKEY_TEXTURE(aiTextureType_SPECULAR, 0), textureFilename) == AI_SUCCESS)
            {

                if (auto texture = assimpScene->GetEmbeddedTexture(textureFilename.C_Str()))
                {
                    std::filesystem::path textureFilepath;
                    GetFBXTexturePath(textureFilepath, filepath, textureFilename.C_Str());

                    if (!VerifIfTextureCacheExist(textureFilepath.filename().string().c_str()))
                        ImportTextureData(textureFilepath, texture);

                    AddTextureDataToModel(model, ETextureType::SPECULAR, textureFilepath);
                }
                else
                {
                    std::filesystem::path textureFilepath(filepath.parent_path());
                    textureFilepath += "/";
                    textureFilepath += textureFilename.C_Str();

                    if (!VerifIfTextureCacheExist(textureFilepath.filename().string().c_str()))
                        ImportTextureData(textureFilepath);

                    AddTextureDataToModel(model, ETextureType::SPECULAR, textureFilepath);
                }
            }

            if (assimpMaterial->Get(AI_MATKEY_TEXTURE(aiTextureType_AMBIENT_OCCLUSION, 0), textureFilename) == AI_SUCCESS)
            {

                if (auto texture = assimpScene->GetEmbeddedTexture(textureFilename.C_Str()))
                {
                    std::filesystem::path textureFilepath;
                    GetFBXTexturePath(textureFilepath, filepath, textureFilename.C_Str());

                    if (!VerifIfTextureCacheExist(textureFilepath.filename().string().c_str()))
                        ImportTextureData(textureFilepath, texture);

                    AddTextureDataToModel(model, ETextureType::AO, textureFilepath);
                }
                else
                {
                    std::filesystem::path textureFilepath(filepath.parent_path());
                    textureFilepath += "/";
                    textureFilepath += textureFilename.C_Str();

                    if (!VerifIfTextureCacheExist(textureFilepath.filename().string().c_str()))
                        ImportTextureData(textureFilepath);

                    AddTextureDataToModel(model, ETextureType::AO, textureFilepath);
                }
            }
        }
    }

    #pragma endregion

    #pragma region Model

    void ImportModelDataRecursive(const aiNode* node, int modelParentIndex, unsigned int& index, int meshIndex,
        const aiScene* scene, std::vector<ImportModelUtils>& models, const std::filesystem::path& filepath)
    {
            ImportModelUtils model{};

            model.m_modelName = filepath.string();
            model.m_modelName += std::string("/");
            model.m_modelName += std::to_string(index);

            model.modelHeader.m_modelNameSize = model.m_modelName.size();
            model.modelHeader.m_index = index;
            model.modelHeader.m_parentIndex = modelParentIndex;

            aiVector3D trs[3];
            node->mTransformation.Decompose(trs[2], trs[1], trs[0]);


            for (int i = 0; i < 3; ++i)
            {
                for (int j = 0; j < 3; ++j)
                {
                    model.modelHeader.m_trs[i].data[j] = trs[i][j];
                }
            }

            ImportMeshData(scene, node, meshIndex, model, filepath);
            ImportMaterialData(scene, node, meshIndex, model, filepath);

            model.modelHeader.m_childrenCount = node->mNumChildren;

            models.push_back(model);

            int indexBuff = index;
            ++index;

            std::vector<unsigned int> childrenIndexs;
            childrenIndexs.reserve(model.modelHeader.m_childrenCount * sizeof(unsigned int));
            for (unsigned int i = 0; i < node->mNumChildren; ++i)
            {
                childrenIndexs.push_back(index);
                ImportModelDataRecursive(node->mChildren[i], model.modelHeader.m_index, index, 0, scene, models, filepath);
            }

            if (node->mNumChildren)
                models[indexBuff].m_childrenIndices.insert(models[indexBuff].m_childrenIndices.end(), childrenIndexs.begin(), childrenIndexs.end());

            if (node->mNumMeshes > 1 && node->mNumMeshes > (unsigned int)meshIndex + 1)
            {
                models[modelParentIndex].modelHeader.m_childrenCount++;
                models[modelParentIndex].m_childrenIndices.push_back(index);
                ImportModelDataRecursive(node, modelParentIndex, index, meshIndex + 1, scene, models, filepath);
            }
    }

    void ImportModelData(const aiScene* scene, std::vector<ImportModelUtils>& models, const std::filesystem::path& filepath)
    {
        unsigned int currentIndex = 0;
        ImportModelDataRecursive(scene->mRootNode, -1, currentIndex, 0, scene, models, filepath);
    }

    void CacheModelData(const std::filesystem::path& filepath, const std::vector<ImportModelUtils>& models)
    {
        FILE* file = nullptr;

        std::string fullFilepath(CCImporter::cacheDirectory);
        fullFilepath += filepath.filename().generic_string();
        fullFilepath += CCImporter::cacheExtension;

        if (!std::filesystem::exists(CCImporter::cacheDirectory))
            std::filesystem::create_directory(CCImporter::cacheDirectory);

        if (fopen_s(&file, fullFilepath.c_str(), "wb"))
        {
            Debug::GetInstance()->AddLog(ELogType::ERROR, std::format("Failed to open/create file : {}", fullFilepath).c_str());
            return;
        }

        size_t modelSize = models.size();
        fwrite(&modelSize, sizeof(size_t), 1, file);

        for (const ImportModelUtils& model : models)
        {
            fwrite(&model.modelHeader, sizeof(ModelHeader), 1, file);
            fwrite(&model.m_modelName[0], model.m_modelName.size(), 1, file);

            if (model.modelHeader.m_childrenCount)
                fwrite(&model.m_childrenIndices[0], model.m_childrenIndices.size() * sizeof(unsigned int), 1, file);

            if (model.modelHeader.m_meshHeader.m_hasMesh)
            {
                fwrite(&model.m_meshName[0], model.m_meshName.size(), 1, file);

                fwrite(&model.m_vertices[0], model.m_vertices.size() * sizeof(Vertex), 1, file);
                fwrite(&model.m_indices[0], model.m_indices.size() * sizeof(unsigned int), 1, file);
            }

            if (model.modelHeader.m_materialHeader.m_hasMaterial)
            {
                fwrite(&model.m_materialName[0], model.m_materialName.size(), 1, file);

                if (model.modelHeader.m_materialHeader.m_texturesCount)
                {

                    fwrite(&model.m_texturesPathSize[0], model.m_texturesPathSize.size() * sizeof(unsigned int), 1, file);
                    fwrite(&model.m_texturesType[0], model.m_texturesType.size() * sizeof(unsigned int), 1, file);
                    fwrite(&model.m_texturesPath[0], model.m_texturesPath.size(), 1, file);
                }
            }
        }

        fclose(file);
    }

    void ImportModel(const std::filesystem::path& filepath, std::vector<ImportModelUtils>& models)
    {
        Assimp::Importer importer = Assimp::Importer();

        const aiScene* scene = importer.ReadFile(filepath.string().c_str(), aiProcess_Triangulate |
            aiProcess_SortByPType |
            aiProcess_GenNormals |
            aiProcess_GenUVCoords |
            aiProcess_FlipUVs |
            aiProcess_CalcTangentSpace);

        if (scene)
        {
            ImportModelData(scene, models, filepath);
            CacheModelData(filepath, models);
        }
        else
        {
            Debug::GetInstance()->AddLog(ELogType::ERROR, std::format("Error parsing : {}", importer.GetErrorString()).c_str());
            return;
        }
    }

    #pragma endregion

}