#include "render_manager.hpp"

#include <glad/gl.h>

#include "resourceManager.hpp"

#include "basic_subpipeline.hpp"

template <>
RenderManager* Singleton<RenderManager>::currentInstance = nullptr;

RenderManager::RenderManager()
{
	if (gladLoaderLoadGL() == 0)
		printf("gladLoaderLoadGL failed\n");
}

void RenderManager::DrawScene()
{
	RenderManager* RM = instance();
	
	if (RM->m_existingSubpipelines.size() > 0)
		InitializePipeline(DefaultRenderingPipeline());

	glClearColor(0.f, 0.f, 0.f, 1.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	for (ASubPipeline* pipeline : RM->m_orderedPipeline)
		pipeline->Execute();

	glUseProgram(0);
}

void RenderManager::InitializePipeline(const PipelineDesc& pipelineDesc)
{
	RenderManager* RM = instance();

	pipelineDesc(RM->m_existingSubpipelines, RM->m_orderedPipeline);
}

RenderManager::PipelineDesc RenderManager::DefaultRenderingPipeline()
{
	return [](const std::unordered_map<std::type_index, ASubPipeline*>& pipelines,
		std::vector<ASubPipeline*>& orderedPipelines)
	{
		ASubPipeline* pipeline = pipelines.find(typeid(BasicSubPipeline))->second;
		orderedPipelines.push_back(pipeline);
	};
}