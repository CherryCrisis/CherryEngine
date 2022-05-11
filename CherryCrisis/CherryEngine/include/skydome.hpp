#pragma once

#include <memory>
#include "sky_renderer.hpp"

class Spheremap;

class Skydome : public SkyRenderer
{
public:
	std::shared_ptr<Mesh>		m_quad;
	std::shared_ptr<Spheremap>	m_spheremap;

	Skydome(Cell* cell);
	~Skydome();

	void RemoveMap() override;
	void RemoveQuad();

	void Load() override;
	void ClearData() override;

	void SubscribeToPipeline(ARenderingPipeline* pipeline) override;
	void UnsubscribeToPipeline(ARenderingPipeline* pipeline) override;
};