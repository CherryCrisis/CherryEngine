#pragma once

#include <vector>

#include <cherry_macros.hpp>

#include "maths.hpp"
#include "renderer.hpp"
#include "resource.hpp"

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

enum class EMeshShape : int
{
	QUAD		= 0,
	CUBE		= 1,
	SPHERE		= 2,
	CYLINDER	= 3,
};

struct GPUMesh
{
	virtual ~GPUMesh() = default; 
};

struct AABB
{
	Vector3 m_center	= Vector3::Zero;
	Vector3 m_extents	= Vector3::Zero;

	AABB() = default;

	AABB(const Vector3& min, const Vector3& max)
		: m_center((max+min) * 0.5f), 
		m_extents(max.x - m_center.x, max.y - m_center.y, max.z - m_center.z) {}

	AABB(const Vector3& inCenter, float iI, float iJ, float iK)
		: m_center{ inCenter }, m_extents{ iI, iJ, iK }
	{}
};

class CCENGINE_API Mesh : public Resource<Mesh>
{
public:
	std::unique_ptr<GPUMesh> m_gpuMesh = nullptr;

	AABB m_aabb;
	std::vector<Vertex>			m_vertices;
	std::vector<unsigned int>	m_indices;

	Mesh(const char* meshName) : Resource<Mesh>(meshName) {}
	virtual ~Mesh() = default;

	static void CreateCylinder(std::shared_ptr<Mesh> mesh, float radius, float halfHeight, float longitude);
	static void CreateSphere(std::shared_ptr<Mesh> mesh, float radius, float latitude, float longitude);
	static void CreateCube(std::shared_ptr<Mesh> mesh, float xHalfRes, float yHalfRes, float zHalfRes);
	static void CreateQuad(std::shared_ptr<Mesh> mesh, float xHalfRes, float yHalfRes);

	static void Load(std::shared_ptr<Mesh> mesh, EMeshShape shapeType, float xHalfRes, float yHalfRes = 0, float zHalfRes = 0);

	static void Load(std::shared_ptr<Mesh> mesh, std::vector<Vertex>& vertices, std::vector<unsigned int>& indices, const AABB& aabb = {});

	void Delete() override;
	void ClearData();

	void Reload(std::vector<Vertex>& vertices, std::vector<unsigned int>& indices, const AABB& aabb);
};

#include "mesh.inl"