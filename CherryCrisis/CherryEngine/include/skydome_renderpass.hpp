#pragma once

#include <glad/gl.h>

#include "rendering_renderpass_interface.hpp"

#include "element_mesh_generator.hpp"

#include "spheremap.hpp"

class Skydome;
class Viewer;

class SkydomeRenderPass : public ARenderingRenderPass, ElementMeshGenerator
{
private:
	Skydome* m_skydome = nullptr;


public:

	struct GPUSkydomeCubemap : GPUCubemapV2
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
	int Subscribe(Skydome* toGenerate);

	template <>
	void Unsubscribe(Skydome* toGenerate);

	void Execute(Framebuffer& framebuffer, Viewer*& viewer);
};
