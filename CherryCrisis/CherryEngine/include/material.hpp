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

	//materialName = modelPath\name
	Material(const char* materialName) : Resource(materialName) {}
public:
	std::shared_ptr<Texture> ambientTexture;
	std::shared_ptr<Texture> albedoTexture;

	~Material();

	static Resource* Create(const char* filepath, const aiMaterial* assimpMaterial);
};
