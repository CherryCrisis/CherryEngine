#pragma once

#include <memory>

#include "renderer.hpp"

#include "cubemap.hpp"
#include "mesh.hpp"

struct Skybox : public ARenderer
{
private:
	void RemoveMesh();
	void RemoveCubemap();

	class Cell* m_cell;

public:
	std::shared_ptr<Mesh>		m_mesh;
	std::shared_ptr<Cubemap>	m_cubemap;

	Skybox(Cell* cell);
	~Skybox();


	void Load();
	void ClearData();

	void SubscribeToPipeline(ARenderingPipeline* pipeline) override;
	void UnsubscribeToPipeline(ARenderingPipeline* pipeline) override;
};