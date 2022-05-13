#pragma once

#include <unordered_set>
#include <map>

#include <glad/gl.h>

#include "rendering_renderpass_interface.hpp"

#include "ebo_tbn_generator.hpp"
#include "texture_generator.hpp"

#include "texture.hpp"
#include "light.hpp"

#include <cherry_macros.hpp>

class UIImage;

class CCENGINE_API UIRenderPass : public ARenderingRenderPass
{
	std::unordered_set<UIImage*>	m_uiImages;

public:
	UIRenderPass(const char* name);


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
	int Subscribe(UIImage* toGenerate);

	template <>
	void Unsubscribe(UIImage* toGenerate);
	
	void Execute(Framebuffer& framebuffer, Viewer*& viewer);
};
