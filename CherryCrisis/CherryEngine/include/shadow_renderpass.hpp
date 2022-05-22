#pragma once

#include <unordered_set>

#include "element_mesh_generator.hpp"
#include "framebuffer.hpp"
#include "light.hpp"
#include "rendering_renderpass_interface.hpp"

class MeshRenderer;


class ShadowRenderPass : public ARenderingRenderPass
{
	std::unordered_set<Light*> m_lights;
	std::unordered_set<MeshRenderer*> m_models;

	ElementMeshGenerator m_meshGenerator;

public:
	struct GPUShadowLight : GPULight
	{
		Framebuffer framebuffer;
		GLuint depthTexID;

		~GPUShadowLight();
	};

	ShadowRenderPass(const char* name);

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
	int Subscribe(Light* toGenerate);

	template <>
	void Unsubscribe(Light* toGenerate);

	template <>
	int Subscribe(MeshRenderer* toGenerate);

	template <>
	void Unsubscribe(MeshRenderer* toGenerate);

	void Execute(Viewer*& viewer);
};