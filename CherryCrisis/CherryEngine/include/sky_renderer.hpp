#pragma once

#include <memory>

#include "renderer.hpp"

#include "cubemap.hpp"
#include "mesh.hpp"

struct CCENGINE_API GPUBRDF { };

class CCENGINE_API SkyRenderer : public ARenderer
{
protected:
	class Cell* m_cell;

public:
	std::shared_ptr<Mesh>		m_cube = nullptr;
	std::shared_ptr<Mesh>		m_quad = nullptr;
	std::shared_ptr<Texture>	m_texture = nullptr;
	std::unique_ptr<GPUBRDF>	m_gpuBRDF = nullptr;

	SkyRenderer(Cell* cell);
	~SkyRenderer();

	void OnSetTexture(std::shared_ptr<Texture> texture);
	void SetTexture(std::shared_ptr<Texture> texture);

	void RemoveCube();
	void RemoveQuad();
	void RemoveTexture();

	void ClearData();

	void SubscribeToPipeline(ARenderingPipeline* pipeline);
	void UnsubscribeToPipeline(ARenderingPipeline* pipeline);
};