#pragma once

#include <CherryMacros.h>

#include <unordered_map>
#include <functional>
#include <memory>

#include "singleton.hpp"

#include "subpipeline_interface.hpp"

class CCENGINE_API RenderManager : public Singleton<RenderManager>
{
	friend class Singleton<RenderManager>;

public:
	using PipelineDesc = std::function<void(const std::unordered_map<std::type_index, ASubPipeline*>&, std::vector<ASubPipeline*>&)>;

private:
	std::unordered_map<std::type_index, ASubPipeline*>	m_existingSubpipelines;
	std::vector<ASubPipeline*> m_orderedPipeline;

	template <class SubPipelineT>
	constexpr SubPipelineT* GetSubpipeline();

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