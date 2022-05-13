#pragma once

#include <glad/gl.h>

#include "rendering_renderpass_interface.hpp"

#include "element_mesh_generator.hpp"

#include "texture.hpp"

class SkyRenderer;
class Viewer;

class SkydomeRenderPass : public ARenderingRenderPass, ElementMeshGenerator
{
private:
	SkyRenderer* m_skyRenderer = nullptr;

	void SetupSkydome();
public:

	struct GPUSkydomeCubemap : GPUTextureCubemap
	{
		GLuint ID = 0u;
	};

	SkydomeRenderPass(const char* name);

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
	int Subscribe(SkyRenderer* toGenerate);

	template <>
	void Unsubscribe(SkyRenderer* toGenerate);

	void Execute(Framebuffer& framebuffer, Viewer*& viewer);
};
