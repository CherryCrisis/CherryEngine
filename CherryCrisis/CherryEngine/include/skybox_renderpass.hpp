#pragma once

#include <glad/gl.h>

#include "rendering_renderpass_interface.hpp"

#include "element_mesh_generator.hpp"

#include "cubemap.hpp"

class Skybox;
class Viewer;

class SkyboxRenderPass : public ARenderingRenderPass, ElementMeshGenerator
{
private:
	Skybox* m_skybox = nullptr;

	struct GPUSkyboxCubemap : GPUCubemap
	{
		GLuint ID = 0u;
	};

public:
	SkyboxRenderPass(const char* name);

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
	int Subscribe(Skybox* toGenerate);

	template <>
	void Unsubscribe(Skybox* toGenerate);

	void Execute(Framebuffer& framebuffer, Viewer*& viewer);
};