#pragma once

#include <vector>

#include "resource.hpp"
#include "maths.hpp"

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

class Mesh : public Resource
{
private :
	std::vector<Vertex>			m_vertices;
	std::vector<unsigned int>	m_indices;

	//meshName = filepath\name
	Mesh(const char* meshName) : Resource(meshName) {}
public :

	~Mesh();
	
	static Resource* Create(const char* modelPath, const aiMesh* assimpMesh);

};
