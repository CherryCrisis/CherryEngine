#include "pch.hpp"

#include "basic_postprocess_renderpass.hpp"

#include "mesh.hpp"
#include "resource_manager.hpp"

BasicPostProcessRenderPass::BasicPostProcessRenderPass(const char* name)
	: APostProcessRenderPass(name, "Assets/Shaders/basicPostProcess.vert", "Assets/Shaders/basicPostProcess.frag")
{
	if (m_program)
		m_callExecute = CCCallback::BindCallback(&BasicPostProcessRenderPass::Execute, this);

	m_quadMesh = ResourceManager::GetInstance()->AddResourceRef<Mesh>("CC_NormalizedQuad", true);

	Mesh::CreateQuad(m_quadMesh, 1.f, 1.f);

	m_meshGenerator.Generate(m_quadMesh.get());

	m_framebuffer.colorTex.width = 1920; m_framebuffer.colorTex.height = 1080;

	// TODO: Use DSA
	// TODO: Optimize
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

	glBindTexture(GL_TEXTURE_2D, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
}

void BasicPostProcessRenderPass::Execute(Framebuffer& framebuffer)
{
	if (framebuffer.colorTex.width != m_framebuffer.colorTex.width ||
		framebuffer.colorTex.height != m_framebuffer.colorTex.height)
	{
		m_framebuffer.colorTex.width = framebuffer.colorTex.width;
		m_framebuffer.colorTex.height = framebuffer.colorTex.height;

		glBindTexture(GL_TEXTURE_2D, m_framebuffer.colorTex.texID);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, m_framebuffer.colorTex.width, m_framebuffer.colorTex.height, 0, GL_RGB, GL_FLOAT, NULL);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	auto gpuMesh = static_cast<ElementMeshGenerator::GPUMeshBasic*>(m_quadMesh->m_gpuMesh.get());

	if (!gpuMesh)
		return;

	glUseProgram(m_program->m_shaderProgram);

	glDisable(GL_DEPTH_TEST);
	glCullFace(GL_BACK);

	glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer.FBO);
	glViewport(0, 0, m_framebuffer.colorTex.width, m_framebuffer.colorTex.height);

	glBindTexture(GL_TEXTURE_2D, framebuffer.colorTex.texID);

	glUniform1f(glGetUniformLocation(m_program->m_shaderProgram, "uOffset"), 1.f / 300.f);
	glUniform1i(glGetUniformLocation(m_program->m_shaderProgram, "uTransformType"), (GLint)transformType);

	glBindVertexArray(gpuMesh->VAO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gpuMesh->EBO);
	glDrawElements(GL_TRIANGLES, gpuMesh->indicesCount, GL_UNSIGNED_INT, nullptr);

	glUseProgram(0);

	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	glBlitNamedFramebuffer(m_framebuffer.FBO, framebuffer.FBO,
						   0, 0, m_framebuffer.colorTex.width, m_framebuffer.colorTex.height,
						   0, 0, framebuffer.colorTex.width, framebuffer.colorTex.height,
						   GL_COLOR_BUFFER_BIT, GL_NEAREST);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
