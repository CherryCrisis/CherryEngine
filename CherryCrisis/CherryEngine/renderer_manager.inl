#pragma once

template<class RendererT>
void SubscribeRenderer(RendererT* renderer)
{
	auto rendererMap = std::make_pair<std::type_index, Renderer*> (typeid(RendererT), renderer);

	m_renderers.insert(resourceMap);
}

template<class RendererT>
void UnsubscribeRenderer(RendererT* renderer)
{
	auto rendererRange = m_resources.equal_range(typeid(RendererT));

	for (auto it = rendererRange.first; it != rendererRange.second; )
	{
		if (it->second == renderer)
			it = rendererRange.erase(it);
		else
			++it;
	}

}