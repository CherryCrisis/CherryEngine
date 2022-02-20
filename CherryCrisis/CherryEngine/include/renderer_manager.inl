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

template <class SubPipelineT, class RendererT>
void RenderManager::ConsumeFromPipeline(RendererT* renderer)
{
	static_assert(std::is_base_of_v<ASubPipeline, SubPipelineT>, "SubPipelineT is not of inherited of ASubPipeline");

	const auto& pipelineIt = m_subpipelines.find(typeid(RendererT));

	SubPipelineT* currentPipeline = static_cast<SubPipelineT*>(pipelineIt->second);

	currentPipeline->Consume(renderer);
}

template<class RendererT>
void RenderManager::SubscribeRenderer(RendererT* renderer)
{
	auto rendererMap = std::make_pair<std::type_index, ARenderer*>(typeid(RendererT), static_cast<ARenderer*>(renderer));

	m_renderers.insert(rendererMap);
}

template<class RendererT>
void RenderManager::UnsubscribeRenderer(RendererT* renderer)
{
	auto rendererRange = m_renderers.equal_range(typeid(RendererT));

	for (auto it = rendererRange.first; it != rendererRange.second; it)
	{
		if (it->second == renderer)
		{
			m_renderers.erase(it->first);
			return;
		}
	}
}

template<class RendererT>
void RenderManager::GetAllRenderers(std::vector<RendererT*>& renderers) const
{
	auto resourceRange = m_renderers.equal_range(typeid(RendererT));

	auto firstIt = resourceRange.first;
	auto lastIt = resourceRange.second;

	for (auto rendererPair = firstIt; rendererPair != lastIt; ++rendererPair)
		renderers.push_back(static_cast<RendererT*>(rendererPair->second));
}
