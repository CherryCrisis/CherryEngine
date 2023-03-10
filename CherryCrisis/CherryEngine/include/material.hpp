#pragma once

#include <map>
#include <memory>

#include <cherry_macros.hpp>

#include "event.hpp"
#include "maths.hpp"
#include "resource.hpp"

class Texture;

namespace CCImporter
{
	struct MaterialArgs;
}

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
	CELSHADE_PALLET,

	COUNT,
};

enum class EPipelineType : unsigned int
{
	LIT,
	PBR,
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
	bool  m_hasNormal = true;
	float m_specularFactor = 1.f;
	float m_metallicFactor = 1.f;
	float m_roughnessFactor = 1.f;
	float m_ao = 1.f;
	float m_clearCoatFactor = 0.f;
	float m_clearCoatRoughnessFactor = 0.f;
	bool  m_hasCelshade = false;

	std::map<ETextureType, std::shared_ptr<Texture>> m_textures;

	EPipelineType m_pipelineType = EPipelineType::LIT;

	Material(const char* materialName) : Resource(materialName) {}
	~Material() = default;

	static void Load(std::shared_ptr<Material> material);

	void Delete() override;
	void Reload(bool saveOnly = false);

	void SetTexture(ETextureType type, const std::shared_ptr<Texture>& newTexture);
	void SetTexture(ETextureType type, const char* filepath);

	Event<std::shared_ptr<Texture>> m_onTextureSet;

};
