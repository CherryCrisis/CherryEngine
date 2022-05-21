#include <pch.hpp>

#include "render_manager.hpp"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h> 
#include <glad/gl.h>

#include "framebuffer.hpp"
#include "viewer.hpp"


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
}

RenderManager::~RenderManager()
{
    glfwTerminate();
}