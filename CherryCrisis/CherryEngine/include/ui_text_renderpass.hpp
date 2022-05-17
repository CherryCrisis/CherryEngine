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

class UIText;

class CCENGINE_API TextRenderPass : public ARenderingRenderPass
{
	std::unordered_set<UIText*>	m_texts;
	unsigned int VAO, VBO;

public:
	TextRenderPass(const char* name);


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
	int Subscribe(UIText* toGenerate);

	template <>
	void Unsubscribe(UIText* toGenerate);

	void Execute(Framebuffer& framebuffer, Viewer*& viewer);
};