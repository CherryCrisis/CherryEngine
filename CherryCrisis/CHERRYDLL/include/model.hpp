#pragma once

#include <vector>
#include <map>

#include "resource.hpp"

class Mesh;
class Material;
class Model;

struct aiScene;
struct aiNode;

class Model : public Resource
{
private:
	Model(const char* filepath) : Resource(filepath) {}

	std::shared_ptr<Mesh> m_mesh;
	std::shared_ptr<Material> m_material;

public:
	~Model();

	static Resource* Create(const char* filepath, const aiScene* assimpScene, const aiNode* assimpNode);
};
