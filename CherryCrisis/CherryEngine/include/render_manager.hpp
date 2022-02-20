#pragma once

#include <unordered_map>
#include <typeindex>
#include <typeinfo>
#include <memory>

#include "singleton.hpp"

#include "engine.h"

#include "renderer.hpp"
#include "subpipeline_interface.hpp"

#include "shaderProgram.hpp"

class CCENGINE_API RenderManager : public Singleton<RenderManager>
{
	friend class Singleton<RenderManager>;

private:
	std::unordered_multimap<std::type_index, ARenderer*>	m_renderers;
	std::unordered_map<std::type_index, ASubPipeline*>		m_subpipelines;

public:
	RenderManager();

	template <class SubPipelineT, class RendererT>
	void GenerateFromPipeline(RendererT* renderer);

	template <class SubPipelineT, class RendererT>
	void ConsumeFromPipeline(RendererT* renderer);

	template<class RendererT>
	void SubscribeRenderer(RendererT* renderer);

	template<class RendererT>
	void UnsubscribeRenderer(RendererT* renderer);

	static void DrawScene();

	template<class RendererT>
	void GetAllRenderers(std::vector<RendererT*>& renderers) const;
};

#include "renderer_manager.inl"