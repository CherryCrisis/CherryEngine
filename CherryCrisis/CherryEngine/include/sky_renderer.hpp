#pragma once

#include <memory>

#include "mesh.hpp"
#include "metadata.hpp"
#include "renderer.hpp"
#include "texture.hpp"

struct CCENGINE_API GPUBRDF { };


class CCENGINE_API SkyRenderer : public ARenderer
{
private:
	ETextureSurface m_currentTextureSurface;
	
	class Cell* m_cell;

	void PopulateMetadatas();
	
public:
	struct GPUSkybox : GPUTextureCubemap
	{
		unsigned int ID = 0u;

		virtual ~GPUSkybox() = default;
	};

	Metapack m_metadatas;

	std::shared_ptr<Mesh>		m_cube = nullptr;
	std::shared_ptr<Mesh>		m_quad = nullptr;
	std::shared_ptr<Texture>	m_texture = nullptr;
	std::unique_ptr<GPUBRDF>	m_gpuBRDF = nullptr;

	SkyRenderer(Cell* cell);
	~SkyRenderer();

	void SetTextureFromPath(std::string);
	std::string GetTexturePath();

	std::unordered_map<std::string, std::shared_ptr<AMetadata>>& GetMetapack() { return m_metadatas.m_metadatas; }

	//Verif is the texture when is reloaded the surface is compatible with skyRenderer
	void OnReloadTexture(Texture* texture);
	void OnSetTexture(std::shared_ptr<Texture> texture);
	void SetTexture(std::shared_ptr<Texture> texture);

	void RemoveCube();
	void RemoveQuad();
	void RemoveTexture();

	void ClearData();

	void SubscribeToPipeline(ARenderingPipeline* pipeline);
	void UnsubscribeToPipeline(ARenderingPipeline* pipeline);

	CCProperty::CopyProperty<SkyRenderer, std::string> m_texturePath{ this, &SkyRenderer::SetTextureFromPath ,& SkyRenderer::GetTexturePath };
};