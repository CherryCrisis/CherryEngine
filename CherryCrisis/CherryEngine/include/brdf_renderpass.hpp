#pragma once

#include <glad/gl.h>

#include "element_mesh_generator.hpp"
#include "rendering_renderpass_interface.hpp"
#include "sky_renderer.hpp"
#include "texture.hpp"

class SkyRenderer;
class Viewer;


class BRDFRenderPass : public ARenderingRenderPass, ElementMeshGenerator
{
private:
	SkyRenderer* m_skyRenderer = nullptr;

	//BRDF Map is generated once
	void GenerateBRDF();

public:
	struct GPUBRDFSphereMap : public GPUBRDF
	{
		const int	resolution = 520;
		GLuint		ID = 0u;
	};

	BRDFRenderPass(const char* name);

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

};
