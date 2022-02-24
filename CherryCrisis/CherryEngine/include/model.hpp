#pragma once

#include <vector>
#include <map>

#include "resource.hpp"
#include "mesh.hpp"
#include "material.hpp"

class Mesh;
class Material;

struct aiScene;
struct aiNode;

class Model : public Resource
{
private:
	Model(const char* filepath) : Resource(filepath) {}

public:
	std::shared_ptr<Mesh>		m_mesh;
	std::shared_ptr<Material>	m_material;

	~Model();

	static Ref<Model> Create(const char* filepath, const aiScene* assimpScene, const aiNode* assimpNode);
};
