#pragma once

template <class SubPipelineT>
constexpr SubPipelineT* RenderManager::GetSubpipeline()
{
	return pipeline->GetSubpipeline<SubPipelineT>();
}

template <class SubPipelineT>
constexpr SubPipelineT* RenderManager::LoadSubpipeline()
{
	return pipeline->LoadSubpipeline<SubPipelineT>();
}

template <class SubPipelineT, class RendererT>
void RenderManager::SubscribeToPipeline(RendererT* renderer)
{
	pipeline->SubscribeToPipeline<SubPipelineT>(renderer);
}

template <class SubPipelineT, class RendererT>
void RenderManager::UnsubscribeToPipeline(RendererT* renderer)
{
	pipeline->UnsubscribeToPipeline<SubPipelineT>(renderer);
}