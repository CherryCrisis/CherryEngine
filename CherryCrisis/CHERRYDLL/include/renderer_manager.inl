#pragma once

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
