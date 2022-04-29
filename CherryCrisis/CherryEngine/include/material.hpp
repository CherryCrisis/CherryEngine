#pragma once

#include <unordered_map>
#include <memory>

#include "cherry_macros.hpp"

#include "resource.hpp"
#include "maths.hpp"
#include "event.hpp"

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
	METALLIC,
	ROUGHNESS,
	SPECULAR,
	AO,
};

class CCENGINE_API Material : public Resource<Material>
{
public:
	Vector3 m_ambient = { 0.2f, 0.2f, 0.2f };
	Vector3 m_diffuse = { 0.8f, 0.8f, 0.8f };
	Vector3 m_specular = { 0.0f, 0.0f, 0.0f };
	Vector3 m_emissive = { 0.0f, 0.0f, 0.0f };
	float	m_shininess = 0.f;

	//-- PBR --//
	float m_specularFactor = 1.f;
	float m_metallicFactor = 1.f;
	float m_roughnessFactor = 1.f;
	float m_ao = 1.f;
	float m_clearCoatFactor = 0.f;
	float m_clearCoatRoughnessFactor = 0.f;

	std::unordered_map<ETextureType, std::shared_ptr<Texture>> m_textures;

	Material(const char* materialName) : Resource(materialName) {}
	~Material() = default;

	static void Load(std::shared_ptr<Material> material, const MaterialArgs& materialArgs);
	static void Load(std::shared_ptr<Material> material) {} //Empty material

	void Delete() override;
	void Reload(const MaterialArgs& materialArgs);

	void SetTexture(ETextureType type, const std::shared_ptr<Texture>& newTexture);
	void SetTexture(ETextureType type, const char* filepath);

	Event<std::shared_ptr<Texture>> m_onTextureSet;

};
