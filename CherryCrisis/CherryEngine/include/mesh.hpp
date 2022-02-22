#pragma once

#include <vector>

#include "resource.hpp"
#include "maths.hpp"

#include "renderer.hpp"

//Assimp
struct aiMesh;

using namespace CCMaths;

struct Vertex
{
	Vector3 position;
	Vector2 uv;
	Vector3 normal;
	Vector3 tangent;
	Vector3 bitangent;
};

struct GPUMesh { };

class Mesh : public Resource
{
private :
	//meshName = filepath\name
	Mesh(const char* meshName) : Resource(meshName) {}

public:
	GPUMesh* m_gpuMesh = nullptr;

	std::vector<Vertex>			m_vertices;
	std::vector<unsigned int>	m_indices;

	static Resource* Create(const char* modelPath, const aiMesh* assimpMesh);
};
