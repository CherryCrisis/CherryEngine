#pragma once

#include "core/panel.hpp"

#include <filesystem>
#include <unordered_map>

#include "Panels/assets_settings.hpp"

class AssetBrowser : public Panel
{
private :
	class AssetNode 
	{
	public:
		bool         m_isDirectory = false;

		uint64_t m_icon    = 0u;
		unsigned int m_ImGuiID = 0u;
	
		std::filesystem::path m_path; // full path from C:/
		std::filesystem::path m_relativePath; // path starting at Assets without filename
		std::string m_filename; // filename without extension
		std::string m_extension; // extension

		std::string GetFullPath() { return m_path.string() + "\\" + m_filename + m_extension; }
		void MoveTo(const std::filesystem::path& newFolder); // newFolder = new Fullpath

	};
	template <typename T>
	class ResourceNode : public AssetNode
	{
		std::shared_ptr<T> m_resource;
	};

	void CheckThings();

	AssetBrowser::AssetNode GenerateNode(const std::filesystem::directory_entry& entry);
	AssetNode* GetNodeByPath(std::filesystem::path path);
	AssetNode* GetNodeByName(const std::string& name);
	AssetNode* GetNodeByFile(const std::string& file); // filename+extension
	std::string Find(std::filesystem::path path);
	std::filesystem::path FindPath(const std::string& folderName);
	// Control Variables
	float m_padding = 16.f;
	float m_thumbnailSize = 64.f;

	//not clean at all but anyway imgui internal logic problem solving
	bool m_deleting = false;
	bool m_renaming = false;
	bool m_creating = false;
	std::string m_renamingName = "";

	std::filesystem::path m_currentDirectory;
	std::filesystem::path m_solutionDirectory;

	std::unordered_map<std::string, AssetNode> m_nodes;
	//-------------------

	AssetNode* m_focusedNode = nullptr;

	//Display the asset list
	void RenderNodes();
	
	std::unique_ptr<AssetsSettings> m_assetsSettings = nullptr;
	void RenderAssetsSettings();
	
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