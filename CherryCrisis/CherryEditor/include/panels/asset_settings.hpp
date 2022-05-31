#pragma once

#include <filesystem>
#include <map>
#include <memory>

#include "core/panel.hpp"
#include "texture_generator.hpp"

enum class ETextureType : unsigned int;

class AssetSettingsDisplayer;
class Material;
class Texture;


class AssetSettings
{
protected :
	bool m_hasSettingsChanged = false;

public :
	AssetSettingsDisplayer* m_assetSettingsDisplayer = nullptr;

	AssetSettings() = default;
	virtual ~AssetSettings() = default;

	//if return false delete assets settings
	virtual void Render() = 0;
};

class AssetSettingsDisplayer : public Panel
{
private:
	std::unique_ptr<AssetSettings> m_assetSettings = nullptr;

public:
	AssetSettingsDisplayer() = default;

	void SetAssetSettings(std::unique_ptr<AssetSettings> assetSettings)
	{ 
		m_assetSettings = std::move(assetSettings); 
		m_assetSettings->m_assetSettingsDisplayer = this;
	}
	
	void Render() override;
	void ContextCallback() override {};
};


class TextureSettings : public AssetSettings
{
private:
	int		m_currentTypeId = 0;
	int		m_currentSurfaceId = 0;
	bool	m_isFlipped = false;

	std::shared_ptr<Texture> m_texture;

public:
	TextureSettings(std::shared_ptr<Texture> texture);

	void Render() override;
};

class MaterialSettings : public AssetSettings
{
private:
	int m_texturePreviewSize = 50;
	int m_textureTypeFoccused = 0;

	std::map<ETextureType, std::shared_ptr<Texture>> m_textures;
	std::shared_ptr<Material> m_material;

	TextureGenerator m_textureGenerator;

	void GenerateTextureList();

public:
	MaterialSettings(std::shared_ptr<Material> material);

	void Render() override;
	void ContextCallback();
};

