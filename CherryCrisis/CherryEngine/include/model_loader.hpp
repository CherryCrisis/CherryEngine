#pragma once

#include <vector>
#include <memory>
#include <string>

#include "model.hpp"
#include "model_base.hpp"
#include "mesh.hpp"
//class Model;
//struct ModelNode;

//class Vertex;

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
        int internalFormat;
        int compressedSize;
        int height;
        int width;
    };

    struct MaterialHeader
    {
        bool            m_hasMaterial;
        size_t          m_matarialId;
        Vector3         m_albedo;
        float           m_shininess;

        unsigned int    m_texturesCount;
    };


    struct ModelHeader
    {
        size_t          m_modelId;
        unsigned int    m_index; //Index in list
        int             m_parentIndex;
        Vector3         m_trs[3];

        unsigned int    m_childrenCount;

        bool            m_hasMesh;
        size_t          m_meshId;
        unsigned int    m_verticesCount;
        unsigned int    m_indicesCount;

        MaterialHeader  m_materialHeader;
    };

    struct ImportModelUtils
    {
        ModelHeader modelHeader;

        std::vector<unsigned int>   m_childrenIndices;
        std::vector<Vertex>         m_vertices;
        std::vector<unsigned int>   m_indices;

        std::vector<unsigned int>   m_texturesPathSize;
        std::vector<unsigned int>   m_texturesType;
        std::vector<char>           m_texturesPath;

        //No used in writing
        std::vector <std::string> m_texturesPathCstr;
    };

    static const char* cacheExtension(".ccfile");
    static const char* cacheDirectory("Cache/");

	void ImportModel(const char* filepath, std::vector<ImportModelUtils>& models);

    //TODO:
    void ImportTexture(const char* filepath, unsigned char** textureData, TextureHeader& textureHeader);

	//void LoadModel(const char* filepath, ModelNode** rootModels, std::vector<std::shared_ptr<Model>>& models);
	//void ReloadModel(const char* filepath, ModelNode** rootModels, std::vector<std::shared_ptr<Model>>& models);
}

