#pragma once

#include <vector>

#include "resource.hpp"
#include "maths.hpp"

#include "renderer.hpp"

#include "cherry_macros.hpp"

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

struct GPUMesh
{
	virtual ~GPUMesh() = default; 
};

class CCENGINE_API Mesh : public Resource<Mesh>
{
public:
	std::unique_ptr<GPUMesh> m_gpuMesh = nullptr;

	std::vector<Vertex>			m_vertices;
	std::vector<unsigned int>	m_indices;

	Mesh(const char* meshName) : Resource<Mesh>(meshName) {}
	virtual ~Mesh() = default;

	static void Load(std::shared_ptr<Mesh> mesh, const aiMesh* assimpMesh);

	static void CreateCube(std::shared_ptr<Mesh> mesh, float xHalfRes, float yHalfRes, float zHalfRes);

	template <typename... Args>
	static void Load(std::shared_ptr<Mesh> mesh, EMeshShape shapeType, Args... args);

	static void Load(std::shared_ptr<Mesh> mesh, std::vector<Vertex>& vertices, std::vector<unsigned int>& indices);

	void Delete() override;

	void Reload(std::vector<Vertex>& vertices, std::vector<unsigned int>& indices);
};

#include "mesh.inl"