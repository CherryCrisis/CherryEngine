#pragma once

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

	std::shared_ptr<Texture> ambientTexture;

	//materialName = modelPath\name
	Material(const char* materialName) : Resource(materialName) {}
public:

	~Material();

	static Resource* Create(const char* filepath, const aiMaterial* assimpMaterial);
};
