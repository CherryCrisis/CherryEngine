#pragma once

#include <unordered_set>

#include <glad/gl.h>

#include "ebo_tbn_generator.hpp"
#include "framebuffer.hpp"
#include "rendering_renderpass_interface.hpp"
#include "texture.hpp"

class MeshRenderer;
class Viewer;


class OutlineRenderPass : public ARenderingRenderPass
{
private:
	std::unordered_set<MeshRenderer*>	    m_meshRenderers;

public:
	OutlineRenderPass(const char* name);

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
	int Subscribe(MeshRenderer* toGenerate);
	template <>
	void Unsubscribe(MeshRenderer* toGenerate);


	void Execute(Viewer*& viewer);
};