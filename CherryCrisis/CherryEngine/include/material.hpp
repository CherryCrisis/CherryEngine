#pragma once

#include <unordered_map>
#include <memory>

#include "cherry_macros.hpp"

#include "resource.hpp"
#include "maths.hpp"

//Forward declaration
namespace CCImporter
{
	struct MaterialHeader;
}
class Texture;

struct MaterialArgs
{
	CCImporter:: MaterialHeader*	m_materialHeader;
	std::vector<std::string>*		m_texturesPath;
	std::vector<unsigned int>*		m_textureType;
};

using namespace CCMaths;

enum class ETextureType : unsigned int
{
	AMBIENT,
	ALBEDO,
};

class CCENGINE_API Material : public Resource<Material>
{
private:
	Vector3 m_albedo;
	float	m_shininess = 0.f;
public:
	Material(const char* materialName) : Resource(materialName) {}
	~Material() = default;

	std::unordered_map<ETextureType, std::shared_ptr<Texture>> textures;
	static void Load(std::shared_ptr<Material> material, const MaterialArgs& materialArgs);

	void Delete() override;
	void Reload(/*const aiMaterial* assimpMaterial*/);

};
