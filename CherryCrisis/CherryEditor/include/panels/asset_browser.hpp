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

		uint64_t m_icon    = 0u;
		unsigned int m_ImGuiID = 0u;
	
		std::filesystem::path m_path;
		std::filesystem::path m_relatiePath;
		std::string m_filename;
		std::string m_extension;
		

		void Render();
	};

	void CheckThings();
	AssetNode* GetNodeByPath(std::filesystem::path path);

	// Control Variables
	float m_padding = 16.f;
	float m_thumbnailSize = 128.f;

	//not clean at all but anyway imgui internal logic problem solving
	bool m_deleting = false;
	bool m_renaming = false;
	std::string m_renamingName = "";

	std::filesystem::path m_currentDirectory;
	std::filesystem::path m_solutionDirectory;

	std::unordered_map<int, AssetNode> m_nodes;
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
	uint64_t m_fileIcon = 0u;
	uint64_t m_browserIcon = 0u;
	//-------------------
};