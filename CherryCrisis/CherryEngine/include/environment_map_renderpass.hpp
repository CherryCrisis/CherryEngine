#pragma once

#include <glad/gl.h>

#include "rendering_renderpass_interface.hpp"

#include "element_mesh_generator.hpp"

#include "spheremap.hpp"

class Skydome;
class Viewer;

class EnvironmentMapRenderPass : public ARenderingRenderPass, ElementMeshGenerator
{
private:
	Skydome* m_skydome = nullptr;
	
	const int width = 1024;
	const int height = 1024;

	struct GPUSkydomeSpheremap : GPUSpheremap
	{
		GLuint ID = 0u;
		GLuint FBO = 0u;
		GLuint RBO = 0u;
	};
public:

	EnvironmentMapRenderPass(const char* name);

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

	void Execute();

};
