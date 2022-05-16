#pragma once

#include <memory>

#include "renderer.hpp"

#include "mesh.hpp"
#include "texture.hpp"
#include "metadata.hpp"

struct CCENGINE_API GPUBRDF { };

class CCENGINE_API SkyRenderer : public ARenderer
{
private:
	class Cell* m_cell;
	ETextureSurface m_currentTextureSurface;

	void PopulateMetadatas();
public:
	Metadata m_metadatas;

	std::shared_ptr<Mesh>		m_cube = nullptr;
	std::shared_ptr<Mesh>		m_quad = nullptr;
	std::shared_ptr<Texture>	m_texture = nullptr;
	std::unique_ptr<GPUBRDF>	m_gpuBRDF = nullptr;

	struct GPUSkybox : GPUTextureCubemap
	{
		unsigned int ID = 0u;

		virtual ~GPUSkybox() = default;
	};

	SkyRenderer(Cell* cell);
	~SkyRenderer();

	void SetTextureFromPath(std::string);
	std::string GetTexturePath();

	//Verif is the texture when is reloaded the surface is compatible with skyRenderer
	void OnReloadTexture(std::shared_ptr<Texture> texture);
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