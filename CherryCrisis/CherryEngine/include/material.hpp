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
	NORMAL_MAP,
};

class CCENGINE_API Material : public Resource<Material>
{
public:
	Vector3 m_ambient = { 0.2f, 0.2f, 0.2f };
	Vector3 m_diffuse = { 0.8f, 0.8f, 0.8f };
	Vector3 m_specular = { 0.0f, 0.0f, 0.0f };
	Vector3 m_emissive = { 0.0f, 0.0f, 0.0f };
	float	m_shininess = 0.f;

	std::unordered_map<ETextureType, std::shared_ptr<Texture>> textures;

	Material(const char* materialName) : Resource(materialName) {}
	~Material() = default;

	static void Load(std::shared_ptr<Material> material, const MaterialArgs& materialArgs);

	void Delete() override;
	void Reload(const MaterialArgs& materialArgs);
};
