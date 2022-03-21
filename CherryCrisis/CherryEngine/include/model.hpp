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

class Model : public Resource<Model>
{

public:
	Model(const char* filepath) : Resource(filepath) {}
	~Model();

	std::shared_ptr<Mesh>		m_mesh;
	std::shared_ptr<Material>	m_material;

	static void Load(std::shared_ptr<Model> model, const aiScene* assimpScene, const aiNode* assimpNode);
};
