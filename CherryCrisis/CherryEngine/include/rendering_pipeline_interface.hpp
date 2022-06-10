#pragma once

#include <string>
#include <typeinfo>
#include <unordered_map>

#include <cherry_macros.hpp>

#include "renderpass_interface.hpp"

struct	Framebuffer;
class	ARenderingRenderPass;
class	APostProcessRenderPass;
class	Viewer;


class CCENGINE_API ARenderingPipeline
{
private:
	std::unordered_map<std::string, ARenderPass*> m_existingRenderpasses;

public:
	virtual ~ARenderingPipeline() {};

	ARenderPass* GetRenderpass(const char* typeName);

	template <class SubPipelineT>
	constexpr SubPipelineT* GetSubpipeline()
	{
		static_assert(std::is_base_of_v<ARenderPass, SubPipelineT>, "SubPipelineT is not inherited of ASubPipeline");

		return static_cast<SubPipelineT*>(GetRenderpass(typeid(SubPipelineT).name()));
	}

	template <class SubPipelineT>
	constexpr SubPipelineT* LoadSubpipeline()
	{
		if (SubPipelineT* subPipeline = GetSubpipeline<SubPipelineT>())
			return subPipeline;

		const char* typeName = typeid(SubPipelineT).name();

		SubPipelineT* rawPtr = new SubPipelineT(typeName);
		m_existingRenderpasses.emplace(typeName, rawPtr);

		return rawPtr;
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

	virtual void Execute(Viewer* viewer, Framebuffer* framebuffer) = 0;
};