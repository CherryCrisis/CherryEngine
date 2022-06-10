#include <pch.hpp>

#include "rendering_pipeline_interface.hpp"


ARenderPass* ARenderingPipeline::GetRenderpass(const char* typeName)
{
	const auto& pipelineIt = m_existingRenderpasses.find(typeName);

	if (pipelineIt == m_existingRenderpasses.end())
		return nullptr;

	return pipelineIt->second;
}
