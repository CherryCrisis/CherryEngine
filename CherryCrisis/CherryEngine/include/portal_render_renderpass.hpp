#pragma once

#include <unordered_set>

#include <glad/gl.h>

#include "rendering_renderpass_interface.hpp"

#include "ebo_tbn_generator.hpp"
#include "texture_generator.hpp"
#include "portal_generator.hpp"

#include "framebuffer.hpp"
#include "texture.hpp"
#include "portal.hpp"
#include "light.hpp"
#include "mesh.hpp"

class Viewer;

class PortalRenderPass : public ARenderingRenderPass
{
private:
	struct GPUBasicPortal : public GPUPortal
	{
		Framebuffer framebuffer;
	};

	std::unordered_set<Portal*>	m_portals;

	std::shared_ptr<Mesh> m_quadMesh;

protected:
	ElementMeshGenerator m_meshGenerator;
	PortalGenerator m_portalGenerator;
	TextureGenerator m_textureGenerator;

public:
	PortalRenderPass(const char* name);

	template <typename RendererT>
	int Subscribe(RendererT* toGenerate)
	{
		static_assert(false, "RendererT generation is not implemented in BasicSubPipeline");
	}

	template <typename RendererT>
	void Unsubscribe(RendererT* toGenerate)
	{
		static_assert(false, "RendererT deletion is not implemented in BasicSubPipeline");
	}

	template <>
	int Subscribe(Portal* toGenerate);

	template <>
	void Unsubscribe(Portal* toGenerate);

	void Execute(Framebuffer& framebuffer, Viewer*& viewer);
};