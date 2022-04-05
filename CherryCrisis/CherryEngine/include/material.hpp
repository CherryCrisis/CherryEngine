#pragma once

#include <unordered_map>
#include <memory>

#include "cherry_macros.hpp"

#include "resource.hpp"
#include "maths.hpp"

//class from assimp
struct aiMaterial;
struct aiScene;

//Forward declaration
class Texture;

using namespace CCMaths;

class CCENGINE_API Material : public Resource<Material>
{
private:
	Vector3 m_albedo;
	float	m_shininess = 0.f;
public:
	Material(const char* materialName) : Resource(materialName) {}
	~Material() = default;

	std::unordered_map<std::string, std::shared_ptr<Texture>> textures;
	static void Load(std::shared_ptr<Material> material, const aiMaterial* assimpMaterial, const aiScene* assimpScene);
	void Delete() override;
	void Reload(const aiMaterial* assimpMaterial);

};
