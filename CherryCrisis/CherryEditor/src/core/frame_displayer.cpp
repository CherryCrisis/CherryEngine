
#include "core/frame_displayer.hpp"

#include <glad/gl.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h> 

#include <imgui.h>

#include "render_manager.hpp"
#include "camera.hpp"
#include "input_manager.hpp"

void FrameDisplayer::UpdateFramebuffer(float width, float height, Camera& camera)
{
	if (!m_isInit) Init();

    if (width != framebuffer.width || height != framebuffer.height)
        UpdateTextureSize(width, height);

    framebuffer.width = width;
    framebuffer.height = height;

    glViewport(0, 0, (GLsizei)width, (GLsizei)height);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer.FBO);
    RenderManager::DrawScene(framebuffer, camera);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FrameDisplayer::Init() 
{
    m_inputs = InputManager::GetInstance();

    glGenFramebuffers(1, &framebuffer.FBO);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer.FBO);

    glGenTextures(1, &m_ViewTex);
    glBindTexture(GL_TEXTURE_2D, m_ViewTex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1920, 1080, 0, GL_RGB, GL_UNSIGNED_INT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_ViewTex, 0);

    glGenRenderbuffers(1, &framebuffer.RBO);
    glBindRenderbuffer(GL_RENDERBUFFER, framebuffer.RBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, 1920, 1080);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, framebuffer.RBO);

    glBindTexture(GL_TEXTURE_2D, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    m_isInit = true;
}

void FrameDisplayer::Render()
{

}

void FrameDisplayer::UpdateTextureSize(float width, float height)
{
    glBindTexture(GL_TEXTURE_2D, m_ViewTex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, (GLsizei)width, (GLsizei)height, 0, GL_RGB, GL_UNSIGNED_INT, NULL);
    glBindTexture(GL_TEXTURE_2D, 0);

}
