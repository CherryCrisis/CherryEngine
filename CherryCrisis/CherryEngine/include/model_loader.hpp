#pragma once

#include <vector>
#include <memory>
#include <string>

#include "model.hpp"
#include "model_base.hpp"
#include "mesh.hpp"

enum class ETextureFormat;



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
        int mipmapsLevel;
        int blockSize;
    };

    struct MaterialHeader
    {
        bool            m_hasMaterial;
        size_t          m_materialNameSize;
        Vector3         m_ambient;
        Vector3         m_diffuse;
        Vector3         m_specular;
        Vector3         m_emissive;
        float           m_shininess;

        unsigned int    m_texturesCount;
    };

    struct MeshHeader
    {
        bool            m_hasMesh;
        size_t          m_meshNameSize;
        unsigned int    m_verticesCount;
        unsigned int    m_indicesCount;
    };

    struct ModelHeader
    {
        size_t          m_modelNameSize;
        unsigned int    m_index; //Index in list
        int             m_parentIndex;
        Vector3         m_trs[3];

        unsigned int    m_childrenCount;

        MeshHeader      m_meshHeader;
        MaterialHeader  m_materialHeader;
    };

    struct ImportModelUtils
    {
        ModelHeader modelHeader;

        std::string                 m_modelName;
        std::string                 m_materialName;
        std::string                 m_meshName;

        std::vector<unsigned int>   m_childrenIndices;
        std::vector<Vertex>         m_vertices;
        std::vector<unsigned int>   m_indices;

        std::vector<unsigned int>   m_texturesPathSize;
        std::vector<unsigned int>   m_texturesType;
        std::vector<char>           m_texturesPath;

        //No used in writing
        std::vector <std::string> m_texturesPathCstr;
    };

    static const char* assetsDirectory("Assets/");
    static const char* cacheExtension(".ccfile");
    static const char* cacheDirectory("Cache/");

	void ImportModel(const std::filesystem::path& filepath, std::vector<ImportModelUtils>& models);
    void ImportTexture(const std::filesystem::path& filepath,
        unsigned char** textureData, TextureHeader& textureHeader, bool flipTexture, ETextureFormat textureFormat);
}

