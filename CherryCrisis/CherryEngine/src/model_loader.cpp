#include <pch.hpp>

#include "model_loader.hpp"

#include <any>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/matrix4x4.h>
#include <format>
#include <fstream>
#include <glad/gl.h>
#include <nvtt/nvtt.h>
#include <stb_image.h>
#include <stb_image_write.h>
#include <string>
#include <yaml-cpp/yaml.h>

#include "debug.hpp"
#include "render_manager.hpp"
#include "resource_manager.hpp"

#include "cubemap.hpp"
#include "material.hpp"
#include "mesh.hpp"
#include "model.hpp"
#include "texture.hpp"
#include "utils.hpp"


namespace YAML
{
    template<>
    struct convert<CCMaths::Vector3>
    {
        static Node encode(const CCMaths::Vector3& rhs)
        {
            Node node;
            node = (std::to_string(rhs.x) + '/' + std::to_string(rhs.y) + '/' + std::to_string(rhs.z));
            return node;
        }

        static bool decode(const Node& node, CCMaths::Vector3& rhs)
        {
            rhs = String::ExtractVector3(node.as<std::string>());
            return true;
        }
    };
}

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

        if (meshIndex > 0)
            model.m_meshName += '_' + meshIndex;

        const aiVector3D& aiMin = assimpMesh->mAABB.mMin;
        const aiVector3D& aiMax = assimpMesh->mAABB.mMax;
        Vector3 min = { aiMin.x, aiMin.y, aiMin.z };
        Vector3 max = { aiMax.x, aiMax.y, aiMax.z };

        model.modelHeader.m_meshHeader.m_aabb = AABB(min, max);

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

    void CacheTextureData(const std::filesystem::path& texturePath, const unsigned char* cacheData, TextureHeader& textureHeader)
    {
        //-- Meta file --//
        std::string fullFilepathMeta(texturePath.string() + ".cctexture");

        YAML::Node yamlSave;

        YAML::Node settingsSave = yamlSave["settings"];
        settingsSave["format"] = static_cast<unsigned int>(textureHeader.internalFormat);
        settingsSave["wrapS"] = static_cast<unsigned int>(textureHeader.wrapS);
        settingsSave["wrapT"] = static_cast<unsigned int>(textureHeader.wrapT);
        settingsSave["wrapR"] = static_cast<unsigned int>(textureHeader.wrapR);
        settingsSave["minFilter"] = static_cast<unsigned int>(textureHeader.minFilter);
        settingsSave["magFilter"] = static_cast<unsigned int>(textureHeader.magFilter);
        settingsSave["surface"] = static_cast<unsigned int>(textureHeader.surface);
        settingsSave["flipped"] = textureHeader.flipped;

        std::ofstream out(fullFilepathMeta.c_str());
        bool opened = out.is_open();
        out << yamlSave;
        out.close();


        //-- Cache file --//
        std::string fullFilepath(CCImporter::cacheDirectory);
        fullFilepath += texturePath.filename().string();
        fullFilepath += CCImporter::cacheExtension;

        FILE* file = nullptr;

        if (!std::filesystem::exists(CCImporter::cacheDirectory))
            std::filesystem::create_directory(CCImporter::cacheDirectory);

        if (fopen_s(&file, fullFilepath.c_str(), "wb")) //w+b = write in binary mode
        {
            Debug::GetInstance()->AddLog(ELogType::ERROR, std::format("{} {}", "Failed to cache texture", fullFilepath).c_str());
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
    
    bool CompressCubemapTexture(nvtt::Context& context, nvtt::Surface& image, const std::filesystem::path& texturePath, TextureHeader& textureHeader, unsigned char** textureData)
    {
        std::string fullFilepath(CCImporter::cacheDirectory);
        fullFilepath += texturePath.filename().string();
        fullFilepath += CCImporter::cacheExtension;

        nvtt::OutputOptions outputOptions;
        outputOptions.setFileName(fullFilepath.c_str());
        nvtt::CompressionOptions compressionOptions;
        SetTextureFormat(compressionOptions, textureHeader);

        textureHeader.mipmapsLevel = image.countMipmaps();

        Vector2 faceSize;
        faceSize.y = image.height() / 3.f;
        faceSize.x = image.width() / 4.f;

        if (faceSize.y != faceSize.x)
        {
            Debug::GetInstance()->AddLog(ELogType::WARNING, std::format("Texture : {} is not a cubemap", texturePath.string()).c_str());
            return false;
        }

        int faceDataSize = 0;

        bool dataSettingsInitialized = false;
        for (int faceX = 0; faceX < 4; ++faceX)
        {
            for (int faceY = 0; faceY < 3; ++faceY)
            {
                if ((faceY == 0 && faceX == 0) || (faceY == 2 && faceX == 0) || (faceY == 0 && faceX == 2)
                    || (faceY == 2 && faceX == 2) || (faceY == 0 && faceX == 3) || (faceY == 2 && faceX == 3))
                    continue;

                Vector2 pos;
                pos.x = faceSize.x * faceX;
                pos.y = faceSize.y * faceY;

                int faceId;

                if ((faceY == 1 && faceX == 2))
                    faceId = 0; //Right
                else if ((faceY == 1 && faceX == 0))
                    faceId = 1; //Left
                else if ((faceY == 0 && faceX == 1))
                    faceId = 2; //Top
                else if ((faceY == 2 && faceX == 1))
                    faceId = 3; //Bottom
                else if ((faceY == 1 && faceX == 1))
                    faceId = 4; //Front
                else if ((faceY == 1 && faceX == 3))
                    faceId = 5; //Back

                nvtt::Surface cubeImage = image.createSubImage((int)pos.x, (int)pos.x + (int)faceSize.x - 1, (int)pos.y, (int)pos.y + (int)faceSize.y - 1, 0, 0);

                OuputHandler outputHandler;
                outputOptions.setOutputHandler(((nvtt::OutputHandler*)&outputHandler));

                context.compress(cubeImage, 0, 0, compressionOptions, outputOptions);

                if (!dataSettingsInitialized)
                {
                    faceDataSize = (int)outputHandler.m_data.size();
                    textureHeader.size = faceDataSize * 6;
                    *textureData = new unsigned char[textureHeader.size];
                    dataSettingsInitialized = true;
                }

                size_t add = static_cast<size_t>(faceDataSize) * static_cast<size_t>(faceId);
                std::move(outputHandler.m_data.begin(), outputHandler.m_data.end(), *textureData + add);
            }
        }

        textureHeader.height = static_cast<int>(faceSize.y) * 6;
        textureHeader.width = static_cast<int>(faceSize.x) * 1;

        return true;
    }

    void CompressTexture(nvtt::Context& context, nvtt::Surface& image, const std::filesystem::path& texturePath, TextureHeader& textureHeader, unsigned char** textureData)
    {
        std::string fullFilepath(CCImporter::cacheDirectory);
        fullFilepath += texturePath.filename().string();
        fullFilepath += CCImporter::cacheExtension;

        nvtt::OutputOptions outputOptions;
        outputOptions.setFileName(fullFilepath.c_str());
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
        textureHeader.flipped = true;

        std::string fullFilepath(CCImporter::cacheDirectory);
        fullFilepath += filepath.filename().string();
        fullFilepath += CCImporter::cacheExtension;

        unsigned char* textureData;

        CompressTexture(context, image, filepath, textureHeader, &textureData);
        CacheTextureData(filepath, textureData, textureHeader);

        delete textureData;
        stbi_image_free(stbiData);
    }

    void ImportTextureData(const std::filesystem::path& filepath)
    {
        unsigned char* data;
        TextureHeader textureHeader;
        ImportTexture(filepath, &data, textureHeader, true, ETextureFormat::RGBA, ETextureSurface::TEXTURE_2D, ETextureWrap::REPEAT, ETextureWrap::REPEAT, ETextureWrap::REPEAT, ETextureFilter::LINEAR_MIPMAP_LINEAR, ETextureFilter::LINEAR);
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

    void AddTextureDataToMaterial(MaterialArgs& materialArgs, ETextureType textureType, const std::filesystem::path& filepath)
    {
        materialArgs.m_materialHeader.m_texturesCount++;
        materialArgs.m_texturesType.push_back((unsigned int)textureType);
        materialArgs.m_texturesPath.push_back(filepath.string());
    }

    void ImportTexture(const std::filesystem::path& filepath, unsigned char** textureData, TextureHeader& textureHeader, bool flipTexture, ETextureFormat textureFormat, ETextureSurface textureSurface,
        ETextureWrap textureWrapS, ETextureWrap textureWrapT, ETextureWrap textureWrapR,
        ETextureFilter textureMinFilter, ETextureFilter textureMagFilter, bool importSettings)
    {
        if (importSettings)
        {
            if (std::filesystem::exists(filepath.string() + ".cctexture"))
            {
                YAML::Node loader = YAML::LoadFile(filepath.string() + ".cctexture");

                YAML::Node settingsLoaded = loader["settings"];

                if (YAML::Node formatNode = settingsLoaded["format"]; formatNode.IsDefined())
                    textureFormat = static_cast<ETextureFormat>(formatNode.as<unsigned int>());

                if (YAML::Node wrapSNode = settingsLoaded["wrapS"]; wrapSNode.IsDefined())
                    textureWrapS = static_cast<ETextureWrap>(wrapSNode.as<unsigned int>());

                if (YAML::Node wrapTNode = settingsLoaded["wrapT"]; wrapTNode.IsDefined())
                    textureWrapT = static_cast<ETextureWrap>(wrapTNode.as<unsigned int>());

                if (YAML::Node wrapRNode = settingsLoaded["wrapR"]; wrapRNode.IsDefined())
                    textureWrapR = static_cast<ETextureWrap>(wrapRNode.as<unsigned int>());

                if (YAML::Node minFilterNode = settingsLoaded["minFilter"]; minFilterNode.IsDefined())
                    textureMinFilter = static_cast<ETextureFilter>(minFilterNode.as<unsigned int>());

                if (YAML::Node magFilterNode = settingsLoaded["magFilter"]; magFilterNode.IsDefined())
                    textureMagFilter = static_cast<ETextureFilter>(magFilterNode.as<unsigned int>());

                textureSurface = static_cast<ETextureSurface>(settingsLoaded["surface"].as<unsigned int>());
                flipTexture = settingsLoaded["flipped"].as<bool>();
            }
        }

        if (!textureData)
            return;

        nvtt::Context context;

        nvtt::Surface image;
        image.load(filepath.string().c_str());

        if (flipTexture)
            image.flipY();

        textureHeader.internalFormat = textureFormat;
        textureHeader.wrapS = textureWrapS;
        textureHeader.wrapT = textureWrapT;
        textureHeader.wrapR = textureWrapR;
        textureHeader.minFilter = textureMinFilter;
        textureHeader.magFilter = textureMagFilter;

        textureHeader.surface = textureSurface;
        textureHeader.width = image.width();
        textureHeader.height = image.height();
        textureHeader.flipped = flipTexture;

        if (textureSurface == ETextureSurface::TEXTURE_CUBEMAP)
        {
            if (!CompressCubemapTexture(context, image, filepath, textureHeader, textureData))
            {
                textureHeader.surface = ETextureSurface::TEXTURE_2D;
                CompressTexture(context, image, filepath, textureHeader, textureData);
            }
        }
        else
        {
            CompressTexture(context, image, filepath, textureHeader, textureData);
        }

        CacheTextureData(filepath, *textureData, textureHeader);
    }
    #pragma endregion

    #pragma region Material

    void GetFBXTexturePath(std::filesystem::path& textureFilepath, const std::filesystem::path& filepath, const char* texturefbxFilename)
    {
        std::filesystem::path texturefbxFilepath(texturefbxFilename);

        std::string textureFilepathStr(filepath.parent_path().string());
        textureFilepathStr += "\\";
        textureFilepathStr += texturefbxFilepath.filename().string();

        size_t lastindex = textureFilepathStr.find_last_of(".");
        textureFilepathStr.erase(lastindex, textureFilepathStr.size());

        textureFilepathStr += ".png";

        textureFilepath = textureFilepathStr;
    }

    void ImportMaterialTextureData(const aiScene* assimpScene, const aiMaterial* assimpMaterial, const aiTextureType assimpTextureType, 
        const ETextureType textureType, MaterialArgs& materialArgs, const std::filesystem::path& filepath)
    {
        aiString textureFilename;
        if (assimpMaterial->GetTexture(assimpTextureType, 0, &textureFilename) == AI_SUCCESS)
        {
            if (auto texture = assimpScene->GetEmbeddedTexture(textureFilename.C_Str()))
            {
                std::filesystem::path textureFilepath;
                GetFBXTexturePath(textureFilepath, filepath, textureFilename.C_Str());

                if (!VerifIfTextureCacheExist(textureFilepath.filename().string().c_str()))
                    ImportTextureData(textureFilepath, texture);

                AddTextureDataToMaterial(materialArgs, textureType, textureFilepath);
            }
            else
            {
                std::filesystem::path textureFilepath(filepath.parent_path());
                textureFilepath += "\\";
                textureFilepath += textureFilename.C_Str();
                textureFilepath = textureFilepath.lexically_normal();
                
                if (!VerifIfTextureCacheExist(textureFilepath.filename().string().c_str()))
                    ImportTextureData(textureFilepath);

                AddTextureDataToMaterial(materialArgs, textureType, textureFilepath);
            }
        }
    }

    void SaveMaterial(const std::filesystem::path& path, const MaterialArgs& materialArgs)
    {
            //-- Save in assets directory --//
            YAML::Node yamlSave;

            YAML::Node settingsSave = yamlSave["settings"];
            settingsSave["ambient"] = materialArgs.m_materialHeader.m_ambient;
            settingsSave["diffuse"] = materialArgs.m_materialHeader.m_diffuse;
            settingsSave["specular"] = materialArgs.m_materialHeader.m_specular;
            settingsSave["emissive"] = materialArgs.m_materialHeader.m_emissive;
            settingsSave["shininess"] = materialArgs.m_materialHeader.m_shininess;
            settingsSave["hasNormal"] = materialArgs.m_materialHeader.m_hasNormal;
            settingsSave["hasCelshade"] = materialArgs.m_materialHeader.m_hasCelshade;
            settingsSave["specularFactor"] = materialArgs.m_materialHeader.m_specularFactor;
            settingsSave["metallicFactor"] = materialArgs.m_materialHeader.m_metallicFactor;
            settingsSave["roughnessFactor"] = materialArgs.m_materialHeader.m_roughnessFactor;
            settingsSave["ao"] = materialArgs.m_materialHeader.m_ao;
            settingsSave["clearCoatFactor"] = materialArgs.m_materialHeader.m_clearCoatFactor;
            settingsSave["clearCoatRoughnessFactor"] = materialArgs.m_materialHeader.m_clearCoatRoughnessFactor;
            settingsSave["pipeline"] = materialArgs.m_materialHeader.m_pipeline;
            settingsSave["texturesCount"] = materialArgs.m_materialHeader.m_texturesCount;

            for (unsigned int i = 0; i < materialArgs.m_materialHeader.m_texturesCount; ++i)
            {
                std::string textureId = std::format("texture_{}", i).c_str();
                std::string textureTypeId = std::format("textureTypes_{}", i);

                settingsSave[textureId.c_str()] = materialArgs.m_texturesPath[i];
                settingsSave[textureTypeId.c_str()] = materialArgs.m_texturesType[i];
            }

            std::ofstream out(path.string().c_str());
            bool opened = out.is_open();
            out << yamlSave;
            out.close();
            settingsSave.reset();
    }

    void SaveMaterial(Material* material)
    {
        MaterialArgs materialArgs;

        unsigned int textureCount = 0;
        for (auto pair : material->m_textures)
        {
            if (pair.second)
            {
                textureCount++;
                materialArgs.m_texturesType.push_back(static_cast<unsigned int>(pair.first));
                materialArgs.m_texturesPath.push_back(pair.second->GetFilepath());
            }
        }

        materialArgs.m_materialHeader =
        {
            .m_ambient = material->m_ambient,
            .m_diffuse = material->m_diffuse,
            .m_specular = material->m_specular,
            .m_emissive = material->m_emissive,
            .m_shininess = material->m_shininess,
            .m_hasNormal = material->m_hasNormal,
            .m_specularFactor = material->m_specularFactor,
            .m_metallicFactor = material->m_metallicFactor,
            .m_roughnessFactor = material->m_roughnessFactor,
            .m_ao = material->m_ao,
            .m_clearCoatFactor = material->m_clearCoatFactor,
            .m_clearCoatRoughnessFactor = material->m_clearCoatRoughnessFactor,
            .m_texturesCount = textureCount,
            .m_pipeline = static_cast<unsigned int>(material->m_pipelineType),
        };

        SaveMaterial(*material->GetFilesystemPath(), materialArgs);
    }

    void ImportMaterialData(const aiScene* assimpScene, const aiNode* assimpNode, int meshIndex, ImportModelUtils& model, const std::filesystem::path& filepath)
    {
        if (assimpNode->mNumMeshes == 0)
        {
            model.modelHeader.m_hasMaterial = false;
            return;
        }

        model.modelHeader.m_hasMaterial = true;

        size_t meshId = (size_t)assimpNode->mMeshes[meshIndex];

        const aiMesh* assimpMesh = assimpScene->mMeshes[meshId];
        const aiMaterial* assimpMaterial = assimpScene->mMaterials[assimpMesh->mMaterialIndex];

        aiString name(assimpMaterial->GetName());

        if (name.length == 0)
            name = std::format("mat_{}", assimpMesh->mMaterialIndex).c_str();

        model.m_materialPath = filepath.parent_path().string();
        model.m_materialPath += "\\";
        model.m_materialPath += name.C_Str();
        model.m_materialPath += CCImporter::materialExtension;
        model.modelHeader.m_materialPathSize = static_cast<unsigned int>(model.m_materialPath.size());

        if (std::filesystem::exists(model.m_materialPath))
            return;

        MaterialArgs materialArgs {};

        float matValue = 0.f;
        if (AI_SUCCESS == assimpMaterial->Get(AI_MATKEY_SHININESS, matValue))
            materialArgs.m_materialHeader.m_shininess = matValue;

        if (AI_SUCCESS == assimpMaterial->Get(AI_MATKEY_SPECULAR_FACTOR, matValue))
            materialArgs.m_materialHeader.m_specularFactor = matValue;

        if (AI_SUCCESS == assimpMaterial->Get(AI_MATKEY_METALLIC_FACTOR, matValue))
            materialArgs.m_materialHeader.m_metallicFactor = matValue;

        if (AI_SUCCESS == assimpMaterial->Get(AI_MATKEY_ROUGHNESS_FACTOR, matValue))
            materialArgs.m_materialHeader.m_roughnessFactor = matValue;

        if (AI_SUCCESS == assimpMaterial->Get(AI_MATKEY_CLEARCOAT_FACTOR, matValue))
            materialArgs.m_materialHeader.m_clearCoatFactor = matValue;

        if (AI_SUCCESS == assimpMaterial->Get(AI_MATKEY_CLEARCOAT_ROUGHNESS_FACTOR, matValue))
            materialArgs.m_materialHeader.m_clearCoatRoughnessFactor = matValue;

        //Material Color
        aiColor3D color(0.f, 0.f, 0.f);
        if (AI_SUCCESS == assimpMaterial->Get(AI_MATKEY_COLOR_AMBIENT, color))
        {
            materialArgs.m_materialHeader.m_ambient = Vector3(color.r, color.g, color.b);
            materialArgs.m_materialHeader.m_ao = color.r;
        }

        if (AI_SUCCESS == assimpMaterial->Get(AI_MATKEY_COLOR_DIFFUSE, color))
            materialArgs.m_materialHeader.m_diffuse = Vector3(color.r, color.g, color.b);

        if (AI_SUCCESS == assimpMaterial->Get(AI_MATKEY_COLOR_SPECULAR, color))
            materialArgs.m_materialHeader.m_specular = Vector3(color.r, color.g, color.b);

        if (AI_SUCCESS == assimpMaterial->Get(AI_MATKEY_COLOR_EMISSIVE, color))
            materialArgs.m_materialHeader.m_emissive = Vector3(color.r, color.g, color.b);


        ImportMaterialTextureData(assimpScene, assimpMaterial, aiTextureType_AMBIENT, ETextureType::AMBIENT, materialArgs, filepath);
        ImportMaterialTextureData(assimpScene, assimpMaterial, aiTextureType_HEIGHT, ETextureType::NORMAL_MAP, materialArgs, filepath);
        ImportMaterialTextureData(assimpScene, assimpMaterial, aiTextureType_DIFFUSE, ETextureType::ALBEDO, materialArgs, filepath);
        ImportMaterialTextureData(assimpScene, assimpMaterial, aiTextureType_METALNESS, ETextureType::METALLIC, materialArgs, filepath);
        ImportMaterialTextureData(assimpScene, assimpMaterial, aiTextureType_DIFFUSE_ROUGHNESS, ETextureType::ROUGHNESS, materialArgs, filepath);
        ImportMaterialTextureData(assimpScene, assimpMaterial, aiTextureType_SPECULAR, ETextureType::SPECULAR, materialArgs, filepath);
        ImportMaterialTextureData(assimpScene, assimpMaterial, aiTextureType_AMBIENT_OCCLUSION, ETextureType::AO, materialArgs, filepath);

        SaveMaterial(model.m_materialPath, materialArgs);
    }

    bool ImportMaterial(const std::filesystem::path& path, MaterialArgs& materialArgs)
    {
        if (!std::filesystem::exists(path))
            return false;

        YAML::Node loader = YAML::LoadFile(path.string().c_str());

        YAML::Node settingsLoaded = loader["settings"];
        materialArgs.m_materialHeader.m_ambient = settingsLoaded["ambient"].as<CCMaths::Vector3>();
        materialArgs.m_materialHeader.m_diffuse = settingsLoaded["diffuse"].as<CCMaths::Vector3>();
        materialArgs.m_materialHeader.m_specular = settingsLoaded["specular"].as<CCMaths::Vector3>();
        materialArgs.m_materialHeader.m_emissive = settingsLoaded["emissive"].as<CCMaths::Vector3>();
        materialArgs.m_materialHeader.m_shininess = settingsLoaded["shininess"].as<float>();

        if (YAML::Node normalNode = settingsLoaded["hasNormal"]; normalNode.IsDefined())
            materialArgs.m_materialHeader.m_hasNormal = normalNode.as<bool>();

        if (YAML::Node celshadeNode = settingsLoaded["hasCelshade"]; celshadeNode.IsDefined())
            materialArgs.m_materialHeader.m_hasCelshade = celshadeNode.as<bool>();

        materialArgs.m_materialHeader.m_specularFactor = settingsLoaded["specularFactor"].as<float>();
        materialArgs.m_materialHeader.m_metallicFactor = settingsLoaded["metallicFactor"].as<float>();
        materialArgs.m_materialHeader.m_roughnessFactor = settingsLoaded["roughnessFactor"].as<float>();
        materialArgs.m_materialHeader.m_ao = settingsLoaded["ao"].as<float>();
        materialArgs.m_materialHeader.m_clearCoatFactor = settingsLoaded["clearCoatFactor"].as<float>();
        materialArgs.m_materialHeader.m_clearCoatRoughnessFactor = settingsLoaded["clearCoatRoughnessFactor"].as<float>();
        materialArgs.m_materialHeader.m_pipeline = settingsLoaded["pipeline"].as<unsigned int>();
        materialArgs.m_materialHeader.m_texturesCount = settingsLoaded["texturesCount"].as<unsigned int>();

        for (unsigned int i = 0; i < materialArgs.m_materialHeader.m_texturesCount; ++i)
        {
            std::string textureId = std::format("texture_{}", i).c_str();
            std::string textureTypeId = std::format("textureTypes_{}", i);

            materialArgs.m_texturesPath.push_back(settingsLoaded[textureId.c_str()].as<std::string>());
            materialArgs.m_texturesType.push_back(settingsLoaded[textureTypeId.c_str()].as<unsigned int>());
        }

        settingsLoaded.reset();
        loader.reset();

        return true;
    }

    #pragma endregion

    #pragma region Model

    void ImportModelDataRecursive(const aiNode* node, int modelParentIndex, unsigned int& index, int meshIndex,
        const aiScene* scene, std::vector<ImportModelUtils>& models, const std::filesystem::path& filepath)
    {
            ImportModelUtils model{};

            model.m_modelName = node->mName.C_Str();

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
                if (modelParentIndex != -1)
                {
                    models[modelParentIndex].modelHeader.m_childrenCount++;
                    models[modelParentIndex].m_childrenIndices.push_back(index);
                }
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

            if (model.modelHeader.m_hasMaterial)
            {
                fwrite(&model.m_materialPath[0], model.m_materialPath.size(), 1, file);
            }
        }

        fclose(file);
    }

    void ImportModel(const std::filesystem::path& filepath, std::vector<ImportModelUtils>& models)
    {
        Assimp::Importer importer = Assimp::Importer();

        const aiScene* scene = importer.ReadFile(filepath.string().c_str(), aiProcess_Triangulate |
            aiProcess_SortByPType |
            aiProcess_ForceGenNormals |
            aiProcess_GenUVCoords |
            aiProcess_FlipUVs |
            aiProcess_CalcTangentSpace |
            aiProcess_GenBoundingBoxes);


        if (scene)
        {
            ImportModelData(scene, models, filepath);
            CacheModelData(filepath, models);
        }
        else
        {
            if (ThreadPool::GetInstance()->GetMainThreadID() == std::this_thread::get_id())
                Debug::GetInstance()->AddLog(ELogType::ERROR, std::format("Error parsing : {}", importer.GetErrorString()).c_str());

            return;
        }
    }

    #pragma endregion

}