#pragma once

#include "cherry_macros.hpp"

#include <unordered_map>
#include <functional>
#include <memory>

#include "singleton.hpp"

#include "rendering_pipeline_interface.hpp"

#include "rendering_pipeline_interface.hpp"

class CCENGINE_API RenderManager : public Singleton<RenderManager>
{
	friend class Singleton<RenderManager>;

public:
	using RenderingRPipelineDesc = std::function<void(ARenderingPipeline*)>;

	ARenderingPipeline* pipeline = nullptr;

public:
	RenderManager();

	static void DrawScene(Framebuffer& framebuffer, Camera& camera);
	
	template <class SubPipelineT>
	constexpr SubPipelineT* GetSubpipeline();

	template <class SubPipelineT>
	constexpr SubPipelineT* LoadSubpipeline();

	template <class SubPipelineT, class RendererT>
	void SubscribeToPipeline(RendererT* renderer);

	template <class SubPipelineT, class RendererT>
	void UnsubscribeToPipeline(RendererT* renderer);
};

#include "renderer_manager.inl"
