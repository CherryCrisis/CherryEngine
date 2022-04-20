
#pragma once

#include <unordered_set>

#include <glad/gl.h>

#include "element_mesh_generator.hpp"
#include "rendering_renderpass_interface.hpp"

#include "texture.hpp"
#include "light.hpp"
#include "mesh.hpp"

class ModelRenderer;
class Viewer;

class PickingRenderPass : public ARenderingRenderPass, ElementMeshGenerator
{
	std::unordered_set<ModelRenderer*>	m_modelRenderers;


public:
	
	struct GPUTextureBasic : GPUTexture
	{
		GLuint ID = 0u;
	};

	PickingRenderPass(const char* name);

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
	int Subscribe(ModelRenderer* toGenerate);

	template <>
	void Unsubscribe(ModelRenderer* toGenerate);

	void Execute(Framebuffer& fb, Viewer*& viewer);
};