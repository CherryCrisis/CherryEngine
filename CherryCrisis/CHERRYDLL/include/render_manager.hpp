#pragma once

#include <unordered_map>
#include <typeindex>
#include <typeinfo>
#include <memory>

#include "singleton.hpp"

#include "engine.h"

#include "renderer.hpp"
#include "subpipeline_interface.hpp"

class CHERRY_API RenderManager : public Singleton<RenderManager>
{
	friend class Singleton<RenderManager>;

private:
	std::unordered_multimap<std::type_index, ARenderer*> m_renderers;
	std::unordered_multimap<std::type_index, ISubPipeline*> m_subpipelines;

public:
	RenderManager();

	template<class RendererT>
	void SubscribeRenderer(RendererT* renderer);

	template<class RendererT>
	void UnsubscribeRenderer(RendererT* renderer);

	static void DrawScene();

	template<class RendererT>
	void GetAllRenderers(std::vector<RendererT*>& renderers) const;
};

#include "renderer_manager.inl"