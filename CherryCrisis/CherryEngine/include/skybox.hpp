#pragma once

#include <memory>

#include "sky_renderer.hpp"

class Cubemap;

class Skybox : public SkyRenderer
{
private:

public:
	std::shared_ptr<Cubemap> m_cubemap;

	Skybox(Cell* cell);
	~Skybox();

	void RemoveMap() override;

	void Load() override;
	void ClearData() override;

	void SubscribeToPipeline(ARenderingPipeline* pipeline) override;
	void UnsubscribeToPipeline(ARenderingPipeline* pipeline) override;
};