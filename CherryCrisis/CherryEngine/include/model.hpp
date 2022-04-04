#pragma once

#include <vector>
#include <map>

#include "cherry_macros.hpp"

#include "resource.hpp"
#include "material.hpp"

#include "cherry_macros.hpp"

class Mesh;
class Material;

struct aiScene;
struct aiNode;

class CCENGINE_API Model : public Resource<Model>
{
public:
	Model(const char* filepath) : Resource(filepath) {}
	~Model() = default;

	std::string					m_modelBasePath;
	std::shared_ptr<Mesh>		m_mesh;
	std::shared_ptr<Material>	m_material;

	static void Load(std::shared_ptr<Model> model, const aiScene* assimpScene, const aiNode* assimpNode, const char* modelBasePath);
	static void Load(std::shared_ptr<Model> model, std::shared_ptr<Mesh> mesh);

	void Delete() override;
	void Reload(const aiScene* assimpScene, const aiNode* assimpNode);
};
