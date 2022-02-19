#pragma once

#include "core/panel.hpp"

#include <filesystem>

class AssetBrowser : public Panel
{
private :
	// Control Variables
	float m_padding = 16.f;
	float m_thumbnailSize = 128.f;

	std::filesystem::path m_currentDirectory;
	//-------------------


	//Refresh the asset list
	void QuerryBrowser();


	//DEPRECATED : Replace with resource manager references
	unsigned int m_fileIcon = 0;
	unsigned int m_browserIcon = 0;
	//-------------------

public:

	void Render() override;

	AssetBrowser();
};