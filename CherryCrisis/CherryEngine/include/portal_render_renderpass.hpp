#pragma once

#include <unordered_set>
#include <array>

#include <glad/gl.h>

#include "ebo_tbn_generator.hpp"
#include "framebuffer.hpp"
#include "light.hpp"
#include "mesh.hpp"
#include "portal.hpp"
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
	int							m_portalRecursionCount = 2;


	struct PortalViewer
	{
		CCMaths::Matrix4	m_modelViewPortal;
		CCMaths::Matrix4	m_viewMatrix;
		CCMaths::Matrix4	m_projectionMatrix;
		FrustumPlanes		m_frustumPlanes;
		CCMaths::Vector3	m_position;
	};

	void ComputePortalView(Portal* portal, Viewer* viewer, Framebuffer* framebuffer, std::vector<PortalViewer>& portalViewers);
	bool ComputePortalFBO(Portal* portal, Viewer* viewer, Framebuffer* framebuffer, int currentRecursion);
	void ComputePortals(std::unordered_set<Portal*>& portals, Viewer* viewer, Framebuffer* framebuffer, int currentRecursion);
	void DrawPortal(Portal* portal, Viewer* viewer, Framebuffer* framebuffer, int currentRecursion);

	[[nodiscard]] PortalViewer SavePortalViewerParams(const Portal* portal);
	void ApplyPortalViewerParams(Portal* portal, const PortalViewer& portalViewer);

protected:
	ElementMeshGenerator m_meshGenerator;
	TextureGenerator m_textureGenerator;

public:
	static std::array<Framebuffer, 2> m_framebuffers;
	static bool m_fboIsInit;

	PortalRenderPass(const char* name);
	~PortalRenderPass();

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