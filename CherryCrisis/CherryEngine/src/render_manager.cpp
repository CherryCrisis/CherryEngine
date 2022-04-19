#include "pch.hpp"

#include "render_manager.hpp"

#include <glad/gl.h>

#include "resource_manager.hpp"

#include "basic_renderpass.hpp"
#include "skybox_renderpass.hpp"
#include "shadow_renderpass.hpp"

#include "framebuffer.hpp"

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

void RenderManager::DrawScene(Framebuffer& framebuffer, Camera& camera)
{
	RenderManager* RM = GetInstance();
	
    // TODO: Move this
    if (RM->m_orderedRenderingRenderpass.size() == 0 && RM->m_existingRenderpasses.size() > 0)
		InitializePipeline(DefaultRenderingRenderpass(), DefaultPostprocessRenderpass());

	for (ARenderingRenderPass* pipeline : RM->m_orderedRenderingRenderpass)
		pipeline->CallOnExecute(framebuffer, camera);

    for (APostProcessRenderPass* pipeline : RM->m_orderedPostprocessRenderpass)
        pipeline->CallOnExecute(framebuffer);

	glUseProgram(0);
}

void RenderManager::InitializePipeline(const RenderingRPDesc& renderingRenderpasses, const PostprocessRPDesc& postprocessRenderpasses)
{
	RenderManager* RM = GetInstance();

    renderingRenderpasses(RM->m_orderedRenderingRenderpass);
    postprocessRenderpasses(RM->m_orderedPostprocessRenderpass);
}

RenderManager::RenderingRPDesc RenderManager::DefaultRenderingRenderpass()
{
	return [&](std::vector<ARenderingRenderPass*>& orderedRenderpasses)
    {
        ARenderingRenderPass* shadow = RenderManager::GetInstance()->LoadSubpipeline<ShadowRenderPass>();
        orderedRenderpasses.push_back(shadow);

        ARenderingRenderPass* lit = RenderManager::GetInstance()->LoadSubpipeline<BasicRenderPass>();
        orderedRenderpasses.push_back(lit);

        ARenderingRenderPass* skybox = RenderManager::GetInstance()->LoadSubpipeline<SkyboxRenderPass>();
        orderedRenderpasses.push_back(skybox);
	};
}

RenderManager::PostprocessRPDesc RenderManager::DefaultPostprocessRenderpass()
{
    return [&](std::vector<APostProcessRenderPass*>& orderedRenderpasses)
    {

    };
}