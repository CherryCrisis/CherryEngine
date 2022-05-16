#pragma once

#include "core/panel.hpp"

#include <filesystem>
#include <unordered_map>
#include <set>
#include <typeinfo>
#include <typeindex>
#include <vector>

#include <glad/gl.h>

#include "resource_manager.hpp"

//Can't forward declaration
#include "texture.hpp"
#include "sound.hpp"
#include "shader.hpp"
#include "material.hpp"
#include "asset_settings.hpp"
#include "cubemap.hpp"


class AResource;
class ModelBase;
class EditorManager;

const std::set<std::string> textureExtensions = { ".jpg", ".png", ".hdr"};
const std::set<std::string> modelExtensions = { ".obj", ".fbx", ".glb", ".gltf" };
const std::set<std::string> shaderExtensions = { ".frag", ".vert" };
const std::set<std::string> soundExtensions = { ".wav" };
const std::string scriptExtensions = ".cs";
const std::string sceneExtensions = ".cherry";
const std::string matExtensions = ".mat";

class AssetBrowser : public Panel
{
private:

	const std::string scriptTemplate =
		R"CS(using CCEngine;

namespace CCScripting
{
    public class %s : Behaviour
    {
        public %s(System.IntPtr cPtr, bool cMemoryOwn)
            : base(cPtr, cMemoryOwn) {}


        //called at the start of the game
        public void Start()
        {

        }

        //called each tick 
        public void Update()
        {

        }
    }
})CS";

	struct AssetNode
	{
		std::shared_ptr<Texture> m_previewTexture;

		std::filesystem::path	m_path; // full path from C:/
		std::filesystem::path	m_relativePath; // path starting at Assets without filename
		std::string				m_filename; // filename without extension
		std::string				m_extension; // extension

		bool					m_isHovered = false;
		AssetBrowser*			m_assetBrowser = nullptr;

		virtual void Rename(const char* newFilepath) = 0;
		virtual void Reload() = 0;
		virtual void Delete() = 0;
		virtual void Action() = 0; //If double clicked action
	};

	struct DirectoryNode : public AssetNode
	{
		std::vector<AssetNode*> m_assetNodes {};
		DirectoryNode*		m_parentDirectory = nullptr;

		void Rename(const char* newFilepath) override {};
		void Reload() override {};
		void Delete() override {};
		void Action() override { m_assetBrowser->SetCurrentDirectory(this); };
	};

	struct EmptyNode : public AssetNode
	{
		void Rename(const char* newFilepath) override {};
		void Reload() override {};
		void Delete() override {};
		void Action() override {};
	};

	struct ScriptNode : public AssetNode
	{
		void Rename(const char* newFilepath) override {};
		void Reload() override {};
		void Delete() override {};
		void Action() override {};

		//last time edited info
		//check if modified
	};


	template <class ResourceT>
	struct ResourceAssetNode : public AssetNode
	{
		std::shared_ptr<ResourceT> m_resource;

		void Reload() override
		{
			Resource<ResourceT>::ReloadResource(m_resource);
		}

		void Rename(const char* newFilepath) override
		{
			ResourceManager::GetInstance()->Rename(typeid(ResourceT), *m_resource->GetFilesystemPath(), newFilepath);
		}

		void Delete() override
		{
			ResourceManager::GetInstance()->Remove<ResourceT>(m_resource->GetFilepath().c_str());
			ResourceManager::GetInstance()->Purge();
		}

		virtual void Action() override = 0;
	};

	struct MaterialNode : public ResourceAssetNode<Material>
	{
		void Action() override
		{
			m_assetBrowser->m_assetSettingsDisplayer->SetAssetSettings(new MaterialSettings(m_resource));
		};
	};

	struct TextureNode : public ResourceAssetNode<Texture>
	{
		void Action() override 
		{
			m_assetBrowser->m_assetSettingsDisplayer->SetAssetSettings(new TextureSettings(m_resource));
		};
	};

	struct ModelNode : public ResourceAssetNode<ModelBase>
	{
		void Action() override {};
	};

	struct SoundNode : public ResourceAssetNode<Sound>
	{
		void Action() override {};
	};

	struct ShaderNode : public ResourceAssetNode<Shader>
	{
		void Action() override {};
	};

	enum class EBrowserAction
	{ 
		NONE,
		RENAMING,
		DELETING,
		CREATING,
	};


	std::filesystem::path m_assetsDirectory;

	DirectoryNode* m_assetsDirectoryNode = nullptr;
	DirectoryNode* m_currentDirectoryNode = nullptr;

	std::map<std::string, std::unique_ptr<AssetNode>> m_assetNodes;
	std::vector<AssetNode*> m_allAssetNode; //To research in all directories

	const float m_padding = 55.f;
	const float m_upPadding = 5.f;
	float m_thumbnailSize = 40.f;
	char m_researchInput[32] = "";

	AssetNode* m_focusedNode = nullptr;
	EBrowserAction m_browserAction = EBrowserAction::NONE;
	std::string m_popupAssetType;

	TextureGenerator m_textureGenerator;

	void SetAssetNode(const std::filesystem::path& path, AssetNode& assetNode);
	AssetNode* RecursiveQuerryBrowser(const std::filesystem::path& m_path, DirectoryNode* parentDirectory); //return m_assetsDirectoryNode 

	void RenderMenuBar();
	void ResizeCell();
	void RenderDirectoryBar();
	void RenderNodes();

	void BrowserActionCreate();
	void BrowserActionRename();
	void BrowserActionDelete();
	void BrowserAction();

	bool DragAndDropTarget(AssetNode* assetNode);

	EditorManager* m_manager = nullptr;
public :

	std::unordered_map<std::string, time_t> m_timeModified;
	AssetBrowser(AssetSettingsDisplayer* assetSettingsDisplayer, EditorManager* manager = nullptr);

	void SetCurrentDirectory(DirectoryNode* directoryNode) 
	{ 
		m_currentDirectoryNode = directoryNode; 
		strcpy_s(m_researchInput, "");
	}

	void Render() override;
	void ContextCallback() override;
	
	void QuerryBrowser(); //Refresh the asset list, return assetsDirectoryNode
	std::string GetCurrentDirectoryPath() { return m_currentDirectoryNode ? m_currentDirectoryNode->m_path.string() : ""; }

	void ReloadScripts();
	void SetPath(const std::filesystem::path& path);
	AssetSettingsDisplayer* m_assetSettingsDisplayer = nullptr;
};
