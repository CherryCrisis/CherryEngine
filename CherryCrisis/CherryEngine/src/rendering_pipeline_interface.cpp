#include <pch.hpp>

#include "rendering_pipeline_interface.hpp"


ARenderingPipeline::~ARenderingPipeline() 
{
	// TODO: See to delete

	/*
	//Delete entries
	for (auto [name, rawPtr] : m_existingRenderpasses)
	{
		ARenderPass* pass = rawPtr;
		m_existingRenderpasses.erase(name);
		delete pass;
	}*/
}

ARenderPass* ARenderingPipeline::GetRenderpass(const char* typeName)
{
	const auto& pipelineIt = m_existingRenderpasses.find(typeName);

	if (pipelineIt == m_existingRenderpasses.end())
		return nullptr;

	//return pipelineIt->second.get();
	return pipelineIt->second;
}
