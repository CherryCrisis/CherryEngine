%include "enums.swg"

%{
	#include "material.hpp"
	#include "maths.hpp"
%}

%include vector3.i
%include texture.i

%nodefaultctor Material;

%include <std_shared_ptr.i>
%shared_ptr(Material)

enum class ETextureType : unsigned int
{
	AMBIENT,
	ALBEDO,
	NORMAL_MAP,
	METALLIC,
	ROUGHNESS,
	SPECULAR,
	AO,

	COUNT,
};

class Material
{
public:
	CCMaths::Vector3 m_ambient = { 0.2f, 0.2f, 0.2f };
	CCMaths::Vector3 m_diffuse = { 0.8f, 0.8f, 0.8f };
	CCMaths::Vector3 m_specular = { 0.0f, 0.0f, 0.0f };
	CCMaths::Vector3 m_emissive = { 0.0f, 0.0f, 0.0f };
	float	m_shininess = 0.f;

	bool  m_hasNormal = true;
	float m_specularFactor = 1.f;
	float m_metallicFactor = 1.f;
	float m_roughnessFactor = 1.f;
	float m_ao = 1.f;
	float m_clearCoatFactor = 0.f;
	float m_clearCoatRoughnessFactor = 0.f;

	void SetTexture(ETextureType type, const std::shared_ptr<Texture>& newTexture);
	void SetTexture(ETextureType type, const char* filepath);
};
