#pragma once

#include <memory>

#include "renderer.hpp"

struct Mesh;
struct Cubemap;

struct Skybox : public ARenderer
{
	std::shared_ptr<Mesh>		m_mesh;
	std::shared_ptr<Cubemap>	m_cubemap;

	Skybox();
};