#pragma once

#include <filesystem>
#include <set>
#include <typeindex>
#include <typeinfo>
#include <unordered_map>
#include <vector>

#include <glad/gl.h>

#include "resource_manager.hpp"

#include "asset_settings.hpp"
#include "cubemap.hpp"
#include "core/panel.hpp"
#include "material.hpp"
#include "shader.hpp"
#include "sound.hpp"
#include "texture.hpp"

class AResource;
class EditorManager;
class Model;


const std::set<std::string> textureExtensions	= { ".jpg", ".png", ".hdr"};
const std::set<std::string> modelExtensions		= { ".obj", ".fbx", ".glb", ".gltf" };
const std::set<std::string> shaderExtensions	= { ".frag", ".vert" };
const std::set<std::string> soundExtensions     = { ".wav" };
const std::set<std::string> fontExtensions		= { ".ttf" };

const std::set<std::string> ignoredExtensions   = { ".cctexture" };

const std::string			scriptExtensions	= ".cs";
const std::string			sceneExtensions		= ".ccscene";
const std::string			matExtensions       = ".ccmat";


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

	struct AAssetNode
	{
		std::shared_ptr<Texture> m_previewTexture;

		std::filesystem::path	m_path;					// full path from C:/
		std::filesystem::path	m_relativePath;			// path starting at Assets without filename
		std::string				m_filename;				// filename without extension
		std::string				m_extension;			// extension
		std::string			    m_fullFilename;
		std::string				m_fullLoweredFilename;

		bool					m_isHovered = false;

		AssetBrowser*			m_assetBrowser = nullptr;

		virtual ~AAssetNode() = default;
		virtual void Rename(const char* newFilepath) = 0;
		virtual void Reload() = 0;
		virtual void Delete() = 0;
		virtual void Action() = 0; //If double clicked action

	};

	struct DirectoryNode : public AAssetNode
	{
		std::vector<AAssetNode*> m_assetNodes {};
		DirectoryNode*			m_parentDirectory = nullptr;

		void Rename(const char* newFilepath) override {};
		void Reload() override {};
		void Delete() override {};
		void Action() override { m_assetBrowser->SetCurrentDirectory(this); };
	};

	struct EmptyNode : public AAssetNode
	{
		void Rename(const char* newFilepath) override {};
		void Reload() override {};
		void Delete() override {};
		void Action() override {};
	};

	struct ScriptNode : public AAssetNode
	{
		void Rename(const char* newFilepath) override {};
		void Reload() override {};
		void Delete() override {};
		void Action() override;
	};

	struct SceneNode : public AAssetNode
	{
		void Rename(const char* newFilepath) override {};
		void Reload() override {};
		void Delete() override {};
		void Action() override;
	};

	template <class ResourceT>
	struct ResourceAssetNode : public AAssetNode
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
			m_assetBrowser->m_assetSettingsDisplayer->SetAssetSettings(std::make_unique<MaterialSettings>(m_resource));
		};
	};

	struct TextureNode : public ResourceAssetNode<Texture>
	{
		void Action() override 
		{
			m_assetBrowser->m_assetSettingsDisplayer->SetAssetSettings(std::make_unique<TextureSettings>(m_resource));
		};
	};

	struct ModelNode : public ResourceAssetNode<Model>
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

	const float m_padding = 55.f;
	const float m_upPadding = 5.f;
	float		m_thumbnailSize = 40.f;
	char		m_researchInput[32] = "";


	std::map<std::string, std::unique_ptr<AAssetNode>> m_assetNodes;

	std::filesystem::path	m_assetsDirectory;
	std::vector<AAssetNode*> m_allAssetNode;		//To search in all directories

	EBrowserAction		m_browserAction = EBrowserAction::NONE;
	std::string			m_popupAssetType;
	TextureGenerator	m_textureGenerator;

	DirectoryNode*	m_assetsDirectoryNode = nullptr;
	DirectoryNode*	m_currentDirectoryNode = nullptr;
	AAssetNode*		m_focusedNode = nullptr;
	EditorManager*	m_manager = nullptr;

	void SetAssetNode(const std::filesystem::path& path, AAssetNode* assetNode);
	AAssetNode* RecursiveQuerryBrowser(const std::filesystem::path& m_path, DirectoryNode* parentDirectory); //return m_assetsDirectoryNode 

	void RenderMenuBar();
	void ResizeCell();
	void RenderNodes();
	void RenderReloadPopup();

	void BrowserActionCreate();
	void BrowserActionRename();
	void BrowserActionDelete();
	void BrowserAction();

	bool DragAndDropTarget(AAssetNode* assetNode);
	bool m_isReloading = false;
	float m_reloadTimer = .5f; // internal

public :
	std::unordered_map<std::string, time_t> m_timeModified;

	AssetSettingsDisplayer* m_assetSettingsDisplayer = nullptr;

	AssetBrowser(AssetSettingsDisplayer* assetSettingsDisplayer, EditorManager* manager = nullptr);

	void SetCurrentDirectory(DirectoryNode* directoryNode) 
	{ 
		m_currentDirectoryNode = directoryNode; 
		strcpy_s(m_researchInput, "");
	}

	void Render() override;
	void ContextCallback() override;

	void QuerryBrowser(); //Refresh the asset list, return assetsDirectoryNode
	
	void ReloadScripts();
	void BindReloadScripts();
	void SetPath(const std::filesystem::path& path);
	
	std::string GetCurrentDirectoryPath() { return m_currentDirectoryNode ? m_currentDirectoryNode->m_path.string() : ""; }
};
