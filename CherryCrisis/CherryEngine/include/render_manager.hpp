#pragma once

#include "cherry_macros.hpp"

#include <unordered_map>
#include <functional>
#include <memory>

#include "singleton.hpp"

#include "renderpass_interface.hpp"

class CCENGINE_API RenderManager : public Singleton<RenderManager>
{
	friend class Singleton<RenderManager>;

public:
	using PipelineDesc = std::function<void(const std::unordered_map<std::type_index, ARenderPass*>&, std::vector<ARenderPass*>&)>;

private:
	std::unordered_map<std::type_index, ARenderPass*>	m_existingSubpipelines;
	std::vector<ARenderPass*> m_orderedPipeline;

	template <class SubPipelineT>
	constexpr SubPipelineT* GetSubpipeline();

	template <class SubPipelineT>
	constexpr SubPipelineT* LoadSubpipeline();

public:
	RenderManager();

	template <class SubPipelineT, class RendererT>
	void GenerateFromPipeline(RendererT* renderer);

	template <class SubPipelineT, class RendererT>
	void RemoveFromPipeline(RendererT* renderer);

	static void DrawScene();

	static PipelineDesc DefaultRenderingPipeline();

	static void InitializePipeline(const PipelineDesc& pipelineDesc);
};

#include "renderer_manager.inl"