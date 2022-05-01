#pragma once
#include <memory>
#include <filesystem>

class Texture;

class AssetsSettings
{
public :

	AssetsSettings() = default;
	virtual ~AssetsSettings() = default;

	//if return false delete assets settings
	virtual bool Update() = 0;
};

class TextureSettings : public AssetsSettings
{
private:
	int m_currentId = 0;
	bool m_settingsChanged = false;

	const char* m_popupName = "Texture Settings";
	std::shared_ptr<Texture> m_texture;

public:
	TextureSettings(std::shared_ptr<Texture> texture);
	bool Update() override;

};

