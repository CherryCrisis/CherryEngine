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

public:
	std::shared_ptr<Mesh>		m_mesh;
	std::shared_ptr<Cubemap>	m_cubemap;

	Skybox();
	~Skybox();

	void Load();
	void SubscribeToRenderPass();
	void UnsubscribeToRenderPass();
};