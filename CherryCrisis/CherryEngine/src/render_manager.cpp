#include "pch.hpp"

#include "render_manager.hpp"

#include <glad/gl.h>

#include "resource_manager.hpp"

#include "basic_renderpass.hpp"
#include "skybox_renderpass.hpp"

template <>
RenderManager* Singleton<RenderManager>::currentInstance = nullptr;

void debugCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam)
{
    auto const src_str = [source]() {
        switch (source)
        {
        case GL_DEBUG_SOURCE_API: return "API";
        case GL_DEBUG_SOURCE_WINDOW_SYSTEM: return "WINDOW SYSTEM";
        case GL_DEBUG_SOURCE_SHADER_COMPILER: return "SHADER COMPILER";
        case GL_DEBUG_SOURCE_THIRD_PARTY: return "THIRD PARTY";
        case GL_DEBUG_SOURCE_APPLICATION: return "APPLICATION";
        case GL_DEBUG_SOURCE_OTHER:
        default:
            return "OTHER";
        }
    }();

    auto const type_str = [type]() {
        switch (type)
        {
        case GL_DEBUG_TYPE_ERROR: return "ERROR";
        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: return "DEPRECATED_BEHAVIOR";
        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR: return "UNDEFINED_BEHAVIOR";
        case GL_DEBUG_TYPE_PORTABILITY: return "PORTABILITY";
        case GL_DEBUG_TYPE_PERFORMANCE: return "PERFORMANCE";
        case GL_DEBUG_TYPE_MARKER: return "MARKER";
        case GL_DEBUG_TYPE_OTHER:
        default:
            return "OTHER";
        }
    }();

    auto const severity_str = [severity]() {
        switch (severity) {
        case GL_DEBUG_SEVERITY_LOW: return "LOW";
        case GL_DEBUG_SEVERITY_MEDIUM: return "MEDIUM";
        case GL_DEBUG_SEVERITY_HIGH: return "HIGH";
        case GL_DEBUG_SEVERITY_NOTIFICATION:
        default:
            return "NOTIFICATION";
        }
    }();

    printf("[%s] OpenGL %s - %s : %s\n", severity_str, src_str, type_str, message);
}

RenderManager::RenderManager()
{
	// TODO: REMOVE THAT
	if (gladLoaderLoadGL() == 0)
		printf("gladLoaderLoadGL failed\n");


	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	glDebugMessageCallback(debugCallback, NULL);

	glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, nullptr, GL_FALSE);


    // TODO: Remove this
    LoadSubpipeline<SkyboxRenderPass>();
}

void RenderManager::DrawScene(const float x, const float y, Camera& camera)
{
	RenderManager* RM = GetInstance();
	
    // TODO: Move this
	if (RM->m_orderedPipeline.size() == 0 && RM->m_existingSubpipelines.size() > 0)
		InitializePipeline(DefaultRenderingPipeline());

	glClearColor(0.f, 0.f, 0.f, 1.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	for (ARenderPass* pipeline : RM->m_orderedPipeline)
		pipeline->CallOnExecute(x, y, camera);

	glUseProgram(0);
}

void RenderManager::InitializePipeline(const PipelineDesc& pipelineDesc)
{
	RenderManager* RM = GetInstance();

	pipelineDesc(RM->m_existingSubpipelines, RM->m_orderedPipeline);
}

RenderManager::PipelineDesc RenderManager::DefaultRenderingPipeline()
{
	return [](const std::unordered_map<std::type_index, ARenderPass*>& pipelines,
		std::vector<ARenderPass*>& orderedPipelines)
	{
        ARenderPass* lit = pipelines.find(typeid(BasicRenderPass))->second;
        orderedPipelines.push_back(lit);

        ARenderPass* skybox = pipelines.find(typeid(SkyboxRenderPass))->second;
        orderedPipelines.push_back(skybox);
	};
}