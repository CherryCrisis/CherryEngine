#pragma once

template <class SubPipelineT>
constexpr SubPipelineT* RenderManager::GetSubpipeline()
{
	static_assert(std::is_base_of_v<ARenderPass, SubPipelineT>, "SubPipelineT is not inherited of ASubPipeline");

	const std::type_index typeID = typeid(SubPipelineT);

	const auto& pipelineIt = m_existingRenderpasses.find(typeID);

	if (pipelineIt == m_existingRenderpasses.end())
		return nullptr;

	return static_cast<SubPipelineT*>(pipelineIt->second);
}

template <class SubPipelineT>
constexpr SubPipelineT* RenderManager::LoadSubpipeline()
{
	SubPipelineT* subPipeline = GetSubpipeline<SubPipelineT>();

	if (subPipeline)
		return subPipeline;

	const std::type_index typeID = typeid(SubPipelineT);

	subPipeline = new SubPipelineT(typeID.name());
	m_existingRenderpasses[typeID] = subPipeline;

	return subPipeline;
}

template <class SubPipelineT, class RendererT>
void RenderManager::SubscribeToPipeline(RendererT* renderer)
{
	LoadSubpipeline<SubPipelineT>()->Subscribe(renderer);
}

template <class SubPipelineT, class RendererT>
void RenderManager::UnsubscribeToPipeline(RendererT* renderer)
{
	if (SubPipelineT* subPipeline = GetSubpipeline<SubPipelineT>(); subPipeline)
		subPipeline->Unsubscribe(renderer);
}