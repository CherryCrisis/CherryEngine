
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

    if (width != m_framebuffer.colorTex.width || height != m_framebuffer.colorTex.height)
        UpdateTextureSize(width, height);

    glViewport(0, 0, (GLsizei)width, (GLsizei)height);

    // TODO: Move this
    float aspect = (float)width / (float)height;
    camera.m_projectionMatrix = Matrix4::Perspective(camera.fovY, aspect, camera.near, camera.far);

    camera.Draw(m_framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FrameDisplayer::Init() 
{
    m_inputs = InputManager::GetInstance();

    glGenFramebuffers(1, &m_framebuffer.FBO);
    glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer.FBO);

    glGenTextures(1, &m_framebuffer.colorTex.texID);
    glBindTexture(GL_TEXTURE_2D, m_framebuffer.colorTex.texID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, m_framebuffer.colorTex.width, m_framebuffer.colorTex.height, 0, GL_RGB, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_framebuffer.colorTex.texID, 0);

    glGenTextures(1, &m_framebuffer.brightnessTex.texID);
    glBindTexture(GL_TEXTURE_2D, m_framebuffer.brightnessTex.texID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, m_framebuffer.brightnessTex.width, m_framebuffer.brightnessTex.height, 0, GL_RGB, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, m_framebuffer.brightnessTex.texID, 0);

    unsigned int attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
    glDrawBuffers(2, attachments);

    glGenRenderbuffers(1, &m_framebuffer.RBO);
    glBindRenderbuffer(GL_RENDERBUFFER, m_framebuffer.RBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, 1920, 1080);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_framebuffer.RBO);

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
    m_framebuffer.colorTex.width  = m_framebuffer.brightnessTex.width = (GLsizei)width;
    m_framebuffer.colorTex.height = m_framebuffer.brightnessTex.height = (GLsizei)height;

    glBindTexture(GL_TEXTURE_2D, m_framebuffer.colorTex.texID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, m_framebuffer.colorTex.width, m_framebuffer.colorTex.height, 0, GL_RGB, GL_FLOAT, NULL);

    glBindTexture(GL_TEXTURE_2D, m_framebuffer.brightnessTex.texID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, m_framebuffer.brightnessTex.width, m_framebuffer.brightnessTex.height, 0, GL_RGB, GL_FLOAT, NULL);

    glBindRenderbuffer(GL_RENDERBUFFER, m_framebuffer.RBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, (GLsizei)width, (GLsizei)height);

    glBindTexture(GL_TEXTURE_2D, 0);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
}
