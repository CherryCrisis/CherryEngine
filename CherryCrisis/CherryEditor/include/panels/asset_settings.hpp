#pragma once
#include <memory>
#include <filesystem>
#include <map>

#include "core/panel.hpp"
#include "texture_generator.hpp"

class Texture;
enum class ETextureType : unsigned int;
class Material;
class Spheremap;
class Cubemap;
class AssetSettingsDisplayer;

class AssetSettings
{
protected :
	bool m_settingsChanged = false;

public :

	AssetSettings() = default;
	virtual ~AssetSettings() = default;

	//if return false delete assets settings
	virtual void Render() = 0;


	AssetSettingsDisplayer* m_assetSettingsDisplayer;
};

class AssetSettingsDisplayer : public Panel
{
private:
	std::unique_ptr<AssetSettings> m_assetSettings = nullptr;

public :
	AssetSettingsDisplayer() = default;

	void SetAssetSettings(AssetSettings* assetSettings) 
	{ 
		m_assetSettings.reset(assetSettings); 
		m_assetSettings->m_assetSettingsDisplayer = this;
	}
	
	void Render() override;
	void ContextCallback() override {};
};


class TextureSettings : public AssetSettings
{
private:
	int m_currentId = 0;
	bool m_isFlipped = false;

	std::shared_ptr<Texture> m_texture;

public:
	TextureSettings(std::shared_ptr<Texture> texture);
	void Render() override;

};

class MaterialSettings : public AssetSettings
{
private:
	std::map<ETextureType, std::shared_ptr<Texture>> m_textures;
	std::shared_ptr<Material> m_material;

	int m_texturePreviewSize = 50;
	int m_textureTypeFoccused = 0;

	TextureGenerator m_textureGenerator;

	void GenerateTextureList();
public:
	MaterialSettings(std::shared_ptr<Material> material);
	void Render() override;
	void ContextCallback();

};

class SpheremapSettings : public AssetSettings
{
private:
	std::shared_ptr<Spheremap> m_spheremap;
	int m_texturePreviewSize = 50;
	bool isHovered = false;

	TextureGenerator m_textureGenerator;
public:
	SpheremapSettings(std::shared_ptr<Spheremap> spheremap);
	void Render() override;
	void ContextCallback();

};

class CubemapSettings : public AssetSettings
{
private:
	std::shared_ptr<Cubemap> m_cubemap;

	int m_texturePreviewSize = 50;
	int m_textureIdFoccused = 0;

	TextureGenerator m_textureGenerator;

public:
	CubemapSettings(std::shared_ptr<Cubemap> material);
	void Render() override;
	void ContextCallback();

};

