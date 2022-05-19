#pragma once

#include "rendering_renderpass_interface.hpp"

#include <unordered_set>

#include "framebuffer.hpp"
#include "element_mesh_generator.hpp"
#include "light.hpp"

class ModelRenderer;
class ShapeRenderer;

class ShadowRenderPass : public ARenderingRenderPass
{
	std::unordered_set<Light*> m_lights;
	std::unordered_set<ModelRenderer*> m_models;
	std::unordered_set<ShapeRenderer*> m_shapes;

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
	int Subscribe(ModelRenderer* toGenerate);

	template <>
	void Unsubscribe(ModelRenderer* toGenerate);

	template <>
	int Subscribe(ShapeRenderer* toGenerate);

	template <>
	void Unsubscribe(ShapeRenderer* toGenerate);

	void Execute(Framebuffer& framebuffer, Viewer*& viewer);
};