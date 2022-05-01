#pragma once

#include "core/panel.hpp"

#include <filesystem>
#include <unordered_map>
#include <set>
#include <glad/gl.h>
#include <typeinfo>
#include <typeindex>

#include "resource_manager.hpp"

//Can't forward declaration
#include "texture.hpp"
#include "sound.hpp"
#include "shader.hpp"

class AResource;
class ModelBase;

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

	struct GPUTexturePreview : public GPUTexture
	{
		GLuint m_ID = 0u;

		void Generate(Texture* texture);
		void Regenerate(Texture* texture);
		void Destroy();

		GPUTexturePreview(Texture* texture);
		virtual ~GPUTexturePreview();
		void OnReload(std::shared_ptr<Texture> texture);
	};

	struct AssetNode
	{
		std::shared_ptr<Texture> m_previewTexture;

		std::filesystem::path	m_path; // full path from C:/
		std::filesystem::path	m_relativePath; // path starting at Assets without filename
		std::string				m_filename; // filename without extension
		std::string				m_extension; // extension

		bool					m_isHovered = false;

		void UploadPreviewTexture();

		virtual void Rename(const char* newFilepath) = 0;
		virtual void Reload() = 0;
		virtual void Delete() = 0;
		virtual void Action() = 0; //If double clicked action
	};

	struct DirectoryNode : public AssetNode
	{
		std::set<AssetNode*> m_assetNodes {};

		AssetBrowser*		m_assetBrowser = nullptr;
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
	};

	template <class ResourceT>
	struct ResourceAssetNode : public AssetNode
	{
		std::shared_ptr<ResourceT> m_resource;

		void ReloadPreviewTexture(std::shared_ptr<ResourceT> resource)
		{
			UploadPreviewTexture();
		}

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

	struct TextureNode : public ResourceAssetNode<Texture>
	{
		void Action() override {};
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

	const std::vector<std::string> m_textureExtensions = {".jpg", ".png"};
	const std::vector<std::string> m_modelExtensions = { ".obj", ".fbx", ".glsl", ".gltf" };
	const std::vector<std::string> m_shaderExtensions = { ".frag", ".vert" };
	const std::vector<std::string> m_soundExtensions = { ".wav" };
	const std::vector<std::string> m_scriptExtensions = { ".cs" };

	std::filesystem::path m_assetsDirectory;

	DirectoryNode* m_assetsDirectoryNode = nullptr;
	DirectoryNode* m_currentDirectoryNode = nullptr;

	std::map<std::string, std::unique_ptr<AssetNode>> m_allAssetNode;

	const float m_padding = 55.f;
	const float m_upPadding = 5.f;
	float m_thumbnailSize = 40.f;
	char m_researchInput[32] = "";

	AssetNode* m_focusedNode = nullptr;
	EBrowserAction m_browserAction = EBrowserAction::NONE;
	std::string m_popupAssetType;


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

public :
	AssetBrowser();

	void SetCurrentDirectory(DirectoryNode* directoryNode) { m_currentDirectoryNode = directoryNode; };

	void Render() override;
	void ContextCallback() override;
	
	void QuerryBrowser(); //Refresh the asset list, return assetsDirectoryNode

};
