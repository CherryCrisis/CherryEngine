#pragma once

template <class SubPipelineT, class RendererT>
void RenderManager::GenerateFromPipeline(RendererT* renderer)
{
	static_assert(std::is_base_of_v<ASubPipeline, SubPipelineT>, "SubPipelineT is not of inherited of ASubPipeline");

	std::type_index typeID = typeid(RendererT);

	const auto& pipelineIt = m_subpipelines.find(typeID);

	SubPipelineT* currentPipeline = nullptr;

	if (pipelineIt == m_subpipelines.end())
	{
		currentPipeline = new SubPipelineT(typeID.name());
		m_subpipelines[typeID] = currentPipeline;
	}
	else
		currentPipeline = static_cast<SubPipelineT*>(pipelineIt->second);

	currentPipeline->Generate(renderer);
}