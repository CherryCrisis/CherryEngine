#include <pch.hpp>

#include "render_manager.hpp"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h> 
#include <glad/gl.h>

#include "framebuffer.hpp"
#include "viewer.hpp"
#include "element_mesh_generator.hpp"

#include "camera_component.hpp"
#include "debug.hpp"


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

    if (type == GL_DEBUG_TYPE_PERFORMANCE)
        return;

    auto const type_logType = [type]() {
        switch (type)
        {
        case GL_DEBUG_TYPE_ERROR:
            return ELogType::ERROR;

        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
        case GL_DEBUG_TYPE_PORTABILITY:
        case GL_DEBUG_TYPE_PERFORMANCE:
            return ELogType::WARNING;

        case GL_DEBUG_TYPE_MARKER:
        case GL_DEBUG_TYPE_OTHER:
        default:
            return ELogType::INFO;
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

    std::string logStr = std::format("[{}] OpenGL {} - {} : {}\n", severity_str, src_str, type_str, message);
    Debug::GetInstance()->AddLog(type_logType, logStr.c_str());
}

RenderManager::RenderManager()
{
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

void RenderManager::GenerateMainWindow(MainWindow& mainWindow) 
{
    mainWindow.shader = ResourceManager::GetInstance()->AddResource<ShaderProgram>("game", true, "Assets/Shaders/game.vert", "Assets/Shaders/game.frag");
    std::shared_ptr<Mesh> m_quadMesh = ResourceManager::GetInstance()->AddResourceRef<Mesh>("CC_NormalizedQuad", true);

    if (!m_quadMesh->m_gpuMesh)
    {
        Mesh::CreateQuad(m_quadMesh, 1.f, 1.f);
        ElementMeshGenerator m_meshGenerator;
        m_meshGenerator.Generate(m_quadMesh.get());
    }

    mainWindow.quad = m_quadMesh->m_gpuMesh.get();
}

void RenderManager::DrawMainWindow(const MainWindow& mainWindow) 
{
    if (CameraComponent* cameraComp = CameraComponent::GetMainCamera())
    {
        Camera* cam = &cameraComp->m_camera;

        auto gpuMesh = static_cast<ElementMeshGenerator::GPUMeshBasic*>(mainWindow.quad);

        glUseProgram(mainWindow.shader->m_shaderProgram);
        glBindVertexArray(gpuMesh->VAO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gpuMesh->EBO);
        glDisable(GL_DEPTH_TEST);

        glBindTexture(GL_TEXTURE_2D, cam->m_framebuffer->colorTex.texID);
        glDrawElements(GL_TRIANGLES, gpuMesh->indicesCount, GL_UNSIGNED_INT, nullptr);

        glBindVertexArray(0);
        glBindTexture(GL_TEXTURE_2D, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }
}