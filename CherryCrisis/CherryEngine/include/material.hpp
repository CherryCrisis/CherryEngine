#pragma once

#include <unordered_map>
#include <memory>

#include "resource.hpp"
#include "maths.hpp"

//class from assimp
struct aiMaterial;

//Forward declaration
class Texture;

using namespace CCMaths;

class Material : public Resource<Material>
{
private:
	Vector3 m_albedo;
	float	m_shininess = 0.f;
public:
	Material(const char* materialName) : Resource(materialName) {}
	~Material();

	std::unordered_map<std::string, std::shared_ptr<Texture>> textures;


	static void Load(std::shared_ptr<Material> material, const aiMaterial* assimpMaterial);
	void Delete() override;
};
