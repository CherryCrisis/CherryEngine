#pragma once

template <class SubPipelineT, class RendererT>
void RenderManager::SubscribeToPipeline(RendererT* renderer)
{
	for (Viewer* viewer : m_viewers)
	{
		if (auto pipeline = viewer->m_pipeline.get())
			renderer->SubscribeToPipeline(pipeline);
	}
}

template <class SubPipelineT, class RendererT>
void RenderManager::UnsubscribeToPipeline(RendererT* renderer)
{
	for (Viewer* viewer : m_viewers)
	{
		if (auto pipeline = viewer->m_pipeline.get())
			renderer->UnsubscribeToPipeline(pipeline);
	}
}