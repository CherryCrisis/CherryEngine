#pragma once

template <class SubPipelineT>
constexpr SubPipelineT* RenderManager::GetSubpipeline()
{
	static_assert(std::is_base_of_v<ARenderPass, SubPipelineT>, "SubPipelineT is not inherited of ASubPipeline");

	const std::type_index typeID = typeid(SubPipelineT);

	const auto& pipelineIt = m_existingSubpipelines.find(typeID);

	if (pipelineIt == m_existingSubpipelines.end())
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
	m_existingSubpipelines[typeID] = subPipeline;

	return subPipeline;
}

template <class SubPipelineT, class RendererT>
void RenderManager::GenerateFromPipeline(RendererT* renderer)
{
	LoadSubpipeline<SubPipelineT>()->Generate(renderer);
}

template <class SubPipelineT, class RendererT>
void RenderManager::RemoveFromPipeline(RendererT* renderer)
{
	if (SubPipelineT* subPipeline = GetSubpipeline<SubPipelineT>(); subPipeline)
		subPipeline->Remove(renderer);
}