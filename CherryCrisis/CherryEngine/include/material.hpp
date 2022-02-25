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

class Material : public Resource
{
private:
	Vector3 m_albedo;
	float	m_shininess = 0.f;

	//materialName = modelPath\name
	Material(const char* materialName) : Resource(materialName) {}
public:
	std::unordered_map<std::string, std::shared_ptr<Texture>> textures;

	~Material();

	static Ref<Material> Create(const char* filepath, const aiMaterial* assimpMaterial);
};
