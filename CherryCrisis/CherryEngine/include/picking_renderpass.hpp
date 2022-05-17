#pragma once

#include <unordered_set>

#include <glad/gl.h>

#include "element_mesh_generator.hpp"
#include "rendering_renderpass_interface.hpp"

#include "texture.hpp"
#include "light.hpp"
#include "mesh.hpp"

class ModelRenderer;
class UIItem;

class Viewer;

class PickingRenderPass : public ARenderingRenderPass
{
	std::unordered_set<ModelRenderer*>	m_modelRenderers;
	std::unordered_set<UIItem*>	m_uiRenderers;

public:
	ElementMeshGenerator m_meshGenerator;

	struct GPUTextureBasic : GPUTexture2D
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

	template <>
	int Subscribe(UIItem* toGenerate);

	template <>
	void Unsubscribe(UIItem* toGenerate);

	void Execute(Framebuffer& fb, Viewer*& viewer);
};