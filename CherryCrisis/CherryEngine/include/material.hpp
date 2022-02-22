#pragma once

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
	Vector4 m_albedo;
	float	m_shininess = 0.f;

	//materialName = modelPath\name
	Material(const char* materialName) : Resource(materialName) {}
public:
	std::shared_ptr<Texture> ambientTex;
	std::shared_ptr<Texture> albedoTex;
	std::shared_ptr<Texture> specuarTex;
	std::shared_ptr<Texture> emissiveTex;
	std::shared_ptr<Texture> normalMap;

	~Material();

	static Resource* Create(const char* filepath, const aiMaterial* assimpMaterial);
};
