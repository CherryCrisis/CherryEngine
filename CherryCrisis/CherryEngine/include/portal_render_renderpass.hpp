#pragma once

#include <unordered_set>

#include <glad/gl.h>

#include "ebo_tbn_generator.hpp"
#include "framebuffer.hpp"
#include "light.hpp"
#include "mesh.hpp"
#include "portal.hpp"
#include "portal_generator.hpp"
#include "rendering_renderpass_interface.hpp"
#include "texture.hpp"
#include "texture_generator.hpp"

class Viewer;

class PortalRenderPass : public ARenderingRenderPass
{
private:
	std::unordered_set<Portal*>	m_portals;
	std::shared_ptr<Mesh>		m_quadMesh;
	bool						m_isExecuted = false;
	int							m_portalRecursionCount = 3;
	int							m_recursionCell = 3;

	struct PortalViewer
	{
		CCMaths::Matrix4	m_modelViewPortal;
		CCMaths::Matrix4	m_viewMatrix;
		CCMaths::Matrix4	m_projectionMatrix;
		FrustumPlanes		m_frustumPlanes;
		CCMaths::Vector3	m_position;
	};

	void ComputePortalView(Portal* portal, Viewer* viewer, Framebuffer* framebuffer, std::vector<PortalViewer>& portalViewers);
	void ComputePortalFBO(Portal* portal, Viewer* viewer, Framebuffer* framebuffer, int currentRecursion);
	void ComputePortals(std::unordered_set<Portal*>& portals, Viewer* viewer, Framebuffer* framebuffer, int currentRecursion);

	[[nodiscard]] PortalViewer SavePortalViewerParams(const Portal* portal);
	void ApplyPortalViewerParams(Portal* portal, const PortalViewer& portalViewer);

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

	void Execute(Viewer* viewer, Framebuffer* framebuffer);
};