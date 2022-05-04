#pragma once
#include <memory>
#include <filesystem>
#include <map>

#include "core/panel.hpp"

class Texture;
enum class ETextureType : unsigned int;
class Material;
class AssetSettings;

class AssetSettingsDisplayer : public Panel
{
private:
	std::unique_ptr<AssetSettings> m_assetSettings = nullptr;

public :
	AssetSettingsDisplayer() = default;

	void SetAssetSettings(AssetSettings* assetSettings) { m_assetSettings.reset(assetSettings); }
	
	void Render() override;
	void ContextCallback() override {};
};

class AssetSettings
{
protected :
	bool m_settingsChanged = false;

public :

	AssetSettings() = default;
	virtual ~AssetSettings() = default;

	//if return false delete assets settings
	virtual void Render() = 0;
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
	std::map<const ETextureType, std::shared_ptr<Texture>> m_textures;
	std::shared_ptr<Material> m_material;


	void GenerateTextureList();
public:
	MaterialSettings(std::shared_ptr<Material> material);
	void Render() override;

};

