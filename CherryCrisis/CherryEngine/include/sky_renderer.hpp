#pragma once

#include <memory>

#include "renderer.hpp"

#include "cubemap.hpp"
#include "mesh.hpp"

class SkyRenderer : public ARenderer
{
protected:
	class Cell* m_cell;

public:
	std::shared_ptr<Mesh> m_cube;

	SkyRenderer(Cell* cell);
	~SkyRenderer();

	void RemoveCube();
	virtual void RemoveMap() = 0;


	virtual void Load() = 0;
	virtual void ClearData() = 0;

	virtual void SubscribeToPipeline(ARenderingPipeline* pipeline) override = 0;
	virtual void UnsubscribeToPipeline(ARenderingPipeline* pipeline) override = 0;
};