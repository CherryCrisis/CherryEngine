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

namespace CCModelLoader
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
    }
    */

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
    };

    struct ImportModelUtils
    {
        ModelHeader modelHeader;

        std::vector<unsigned int>   m_childrenIndices;
        std::vector<Vertex>         m_vertices;
        std::vector<unsigned int>   m_indices;
    };

	void ImportModel(const char* filepath, std::vector<ImportModelUtils>& models);

	//void LoadModel(const char* filepath, ModelNode** rootModels, std::vector<std::shared_ptr<Model>>& models);
	//void ReloadModel(const char* filepath, ModelNode** rootModels, std::vector<std::shared_ptr<Model>>& models);
}

