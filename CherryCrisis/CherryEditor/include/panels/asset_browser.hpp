#pragma once

#include "core/panel.hpp"

#include <filesystem>
#include <unordered_map>

class AssetBrowser : public Panel
{
private :
	class AssetNode 
	{
	public:
		bool         m_isDirectory = false;

		unsigned int m_icon    = 0u;
		unsigned int m_ImGuiID = 0u;
	
		std::filesystem::path m_path;
		std::string m_filename;

		void Render();
	};

	void CheckThings();


	// Control Variables
	float m_padding = 16.f;
	float m_thumbnailSize = 128.f;

	std::filesystem::path m_currentDirectory;

	std::unordered_map<int,  AssetNode> m_nodes;
	//-------------------

	AssetNode* m_focusedNode = nullptr;

	//Display the asset list
	void RenderNodes();

	
public:

	void Render() override;
	void ContextCallback() override;


	//Refresh the asset list
	void QuerryBrowser();

	AssetBrowser();

	//DEPRECATED : Replace with resource manager references
	unsigned int m_fileIcon = 0u;
	unsigned int m_browserIcon = 0u;
	//-------------------
};