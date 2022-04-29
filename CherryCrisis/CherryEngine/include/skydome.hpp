#pragma once

#include <memory>
#include "renderer.hpp"

class Mesh;
class Spheremap;

class Skydome : public ARenderer
{
private:
	void RemoveMesh();
	void RemoveCubemap();

	class Cell* m_cell;

public:
	std::shared_ptr<Mesh>		m_quad;
	std::shared_ptr<Mesh>		m_mesh;
	std::shared_ptr<Spheremap>	m_spheremap;

	Skydome(Cell* cell);
	~Skydome();


	void Load();
	void ClearData();

	void SubscribeToPipeline(ARenderingPipeline* pipeline) override;
	void UnsubscribeToPipeline(ARenderingPipeline* pipeline) override;
};