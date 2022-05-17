#pragma once
#include <memory>
#include <filesystem>
#include <map>

#include "core/panel.hpp"
#include "texture_generator.hpp"

class Texture;
enum class ETextureType : unsigned int;
class Material;
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


	AssetSettingsDisplayer* m_assetSettingsDisplayer = nullptr;
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
	int m_currentTypeId = 0;
	int m_currentSurfaceId = 0;
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

