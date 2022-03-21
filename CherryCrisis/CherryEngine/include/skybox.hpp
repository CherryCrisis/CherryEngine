#pragma once

#include <memory>

#include "renderer.hpp"

#include "cubemap.hpp"
#include "mesh.hpp"

struct Skybox : public ARenderer
{
	std::shared_ptr<Mesh>		m_mesh;
	std::shared_ptr<Cubemap>	m_cubemap;

	Skybox();
	
	void Load();
};