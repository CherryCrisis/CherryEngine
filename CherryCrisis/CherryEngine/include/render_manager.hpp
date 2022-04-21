#pragma once

#include "cherry_macros.hpp"

#include <set>
#include <memory>

#include "singleton.hpp"

struct Framebuffer;
class Viewer;

class CCENGINE_API RenderManager : public Singleton<RenderManager>
{
	friend class Singleton<RenderManager>;

public:
	std::set<Viewer*> m_viewers;

	RenderManager();

	template <class SubPipelineT, class RendererT>
	void SubscribeToPipeline(RendererT* renderer);

	template <class SubPipelineT, class RendererT>
	void UnsubscribeToPipeline(RendererT* renderer);

	static void DrawScene(Framebuffer& framebuffer, Viewer* viewer);
};

#include "renderer_manager.inl"