#pragma once

#include <unordered_map>
#include <typeinfo>

#include "renderpass_interface.hpp"

struct Framebuffer;
struct Viewer;

class ARenderingRenderPass;
class APostProcessRenderPass;

class ARenderingPipeline
{
	std::unordered_map<std::type_index, ARenderPass*>	m_existingRenderpasses;
public:
	template <class SubPipelineT>
	constexpr SubPipelineT* GetSubpipeline()
	{
		static_assert(std::is_base_of_v<ARenderPass, SubPipelineT>, "SubPipelineT is not inherited of ASubPipeline");

		const std::type_index typeID = typeid(SubPipelineT);

		const auto& pipelineIt = m_existingRenderpasses.find(typeID);

		if (pipelineIt == m_existingRenderpasses.end())
			return nullptr;

		return static_cast<SubPipelineT*>(pipelineIt->second);
	}

	template <class SubPipelineT>
	constexpr SubPipelineT* LoadSubpipeline()
	{
		SubPipelineT* subPipeline = GetSubpipeline<SubPipelineT>();

		if (subPipeline)
			return subPipeline;

		const std::type_index typeID = typeid(SubPipelineT);

		// TODO: Add clear / use smart pointers
		subPipeline = new SubPipelineT(typeID.name());
		m_existingRenderpasses[typeID] = subPipeline;

		return subPipeline;
	}

	template <class SubPipelineT, class RendererT>
	void SubscribeToPipeline(RendererT* renderer)
	{
		LoadSubpipeline<SubPipelineT>()->Subscribe(renderer);
	}

	template <class SubPipelineT, class RendererT>
	void UnsubscribeToPipeline(RendererT* renderer)
	{
		if (SubPipelineT* subPipeline = GetSubpipeline<SubPipelineT>(); subPipeline)
			subPipeline->Unsubscribe(renderer);
	}

	virtual void Execute(Framebuffer& frambuffer, Viewer* viewer) = 0;
};