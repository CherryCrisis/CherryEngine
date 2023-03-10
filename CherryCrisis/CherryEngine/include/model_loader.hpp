#pragma once

#include <memory>
#include <string>
#include <vector>

#include "cherry_macros.hpp"
#include "mesh.hpp"
#include "model.hpp"

enum class ETextureFormat;
enum class ETextureSurface;
enum class ETextureFilter;
enum class ETextureWrap;
class Cubemap;
class Spheremap;


namespace CCImporter
{
    /*
    Order to load model from cache
    - read modelCount

    for(modelCount)
    {
        read ModelHeader

        if (hasChildren)
            read childrenIndex data

        if (hasMesh)
            read vertices and indices

        if (hasMaterial)
        {
            read texturesPathSize, m_texturesType

            for (i; textureCount)
            {
                read texturesPath with texturesPathSize[i]
            }
        }
    }
    */

    struct TextureHeader
    {
        int height;
        int width;
        int size;
        ETextureFormat internalFormat;
        ETextureSurface surface;
        ETextureWrap wrapS;
        ETextureWrap wrapT;
        ETextureWrap wrapR;
        ETextureFilter minFilter;
        ETextureFilter magFilter;
        int mipmapsLevel;
        int blockSize;
        bool flipped;
    };

    struct MaterialHeader
    {
        Vector3         m_ambient = Vector3::Zero;
        Vector3         m_diffuse = Vector3::Zero;
        Vector3         m_specular = Vector3::Zero;
        Vector3         m_emissive = Vector3::Zero;
        float           m_shininess = 0.f;

        //-- PBR --//
        bool    m_hasNormal = true;
        bool    m_hasCelshade = false;
        float   m_specularFactor = 1.f;
        float   m_metallicFactor = 1.f;
        float   m_roughnessFactor = 1.f;
        float   m_ao = 1.f;
        float   m_clearCoatFactor = 0.f;
        float   m_clearCoatRoughnessFactor = 0.f;

        unsigned int    m_texturesCount = 0;
        unsigned int    m_pipeline = 0;
    };

    struct MaterialArgs
    {
        MaterialHeader m_materialHeader;
        std::vector<std::string> m_texturesPath;
        std::vector<unsigned int> m_texturesType;
    };

    struct MeshHeader
    {
        bool            m_hasMesh;
        size_t          m_meshNameSize;
        unsigned int    m_verticesCount;
        unsigned int    m_indicesCount;

        //-- AABB --//
        AABB            m_aabb;

    };

    struct ModelHeader
    {
        size_t          m_modelNameSize;
        unsigned int    m_index; //Index in list
        int             m_parentIndex;
        Vector3         m_trs[3];

        unsigned int    m_childrenCount;

        MeshHeader      m_meshHeader;

        bool            m_hasMaterial;
        unsigned int    m_materialPathSize;
    };

    struct ImportModelUtils
    {
        ModelHeader modelHeader;

        std::string                 m_modelName;
        std::string                 m_materialPath;
        std::string                 m_meshName;

        std::vector<unsigned int>   m_childrenIndices;
        std::vector<Vertex>         m_vertices;
        std::vector<unsigned int>   m_indices;
    };

    static const char* assetsDirectory("Assets/");
    static const char* cacheDirectory("Cache/");
    static const char* cacheExtension(".ccfile");
    static const char* cacheMaterialExtension(".ccmat");
    static const char* materialExtension(".ccmat");
    static const char* spheremapExtension(".ccspheremap");
    static const char* cubemapExtension(".cccubemap");

	void ImportModel(const std::filesystem::path& filepath, std::vector<ImportModelUtils>& models);
    void ImportTexture(const std::filesystem::path& filepath,
        unsigned char** textureData, TextureHeader& textureHeader, bool flipTexture, ETextureFormat textureFormat, ETextureSurface textureSurface,
        ETextureWrap textureWrapS, ETextureWrap textureWrapT, ETextureWrap textureWrapR,
        ETextureFilter textureMinFilter, ETextureFilter textureMagFilter, bool importSettings = true);

    void CCENGINE_API SaveMaterial(Material* material);
    bool ImportMaterial(const std::filesystem::path& path, MaterialArgs& materialArgs);
}

