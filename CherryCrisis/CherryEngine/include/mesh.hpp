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

enum class EMeshShape
{
	PLANE,
	CUBE,
	SPHERE,
};

struct GPUMesh { };

class Mesh : public Resource
{
private :
	//meshName = filepath\name
	Mesh(const char* meshName) : Resource(meshName) {}

public:
	~Mesh() {}
	GPUMesh* m_gpuMesh = nullptr;

	std::vector<Vertex>			m_vertices;
	std::vector<unsigned int>	m_indices;

	static Ref<Mesh> Create(const char* modelPath, const aiMesh* assimpMesh);

	static Ref<Mesh> CreateCube(const char* cubeName, float xHalfRes, float yHalfRes, float zHalfRes);

	template <typename... Args>
	static Ref<Mesh> Create(const char* shapeName, EMeshShape shapeType, Args... args);

	static Ref<Mesh> Create(const char* modelName, std::vector<Vertex>& vertices, std::vector<unsigned int>& indices);
};

#include "mesh.inl"