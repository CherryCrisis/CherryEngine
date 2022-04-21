#include "pch.hpp"

#include "basic_postprocess_renderpass.hpp"

#include "mesh.hpp"
#include "resource_manager.hpp"

BasicPostProcessRenderPass::BasicPostProcessRenderPass(const char* name)
	: APostProcessRenderPass(name, "Assets/basicPostProcess.vert", "Assets/basicPostProcess.frag")
{
	if (m_program)
		m_callExecute = CCCallback::BindCallback(&BasicPostProcessRenderPass::Execute, this);

	m_quadMesh = ResourceManager::GetInstance()->AddResourceRef<Mesh>("CC_NormalizedQuad", true);

	Mesh::CreateQuad(m_quadMesh, 1.f, 1.f);

	Generate(m_quadMesh.get());

	m_framebuffer.colorTex.width = 1920; m_framebuffer.colorTex.height = 1080;

	// TODO: Use DSA
	// TODO: Optimize
	glGenFramebuffers(1, &m_framebuffer.FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer.FBO);

	glGenTextures(1, &m_framebuffer.colorTex.texID);
	glBindTexture(GL_TEXTURE_2D, m_framebuffer.colorTex.texID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_framebuffer.colorTex.width, m_framebuffer.colorTex.height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_framebuffer.colorTex.texID, 0);

	glBindTexture(GL_TEXTURE_2D, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
}

void BasicPostProcessRenderPass::Execute(Framebuffer& frambuffer)
{
	if (frambuffer.colorTex.width != m_framebuffer.colorTex.width ||
		frambuffer.colorTex.height != m_framebuffer.colorTex.height)
	{
		m_framebuffer.colorTex.width = frambuffer.colorTex.width; m_framebuffer.colorTex.height = frambuffer.colorTex.height;

		glBindTexture(GL_TEXTURE_2D, m_framebuffer.colorTex.texID);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_framebuffer.colorTex.width, m_framebuffer.colorTex.height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);

		glBindTexture(GL_TEXTURE_2D, 0);
	}

	auto gpuMesh = static_cast<GPUMeshBasic*>(m_quadMesh->m_gpuMesh.get());

	if (!gpuMesh)
		return;

	glUseProgram(m_program->m_shaderProgram);

	glDisable(GL_DEPTH_TEST);
	glCullFace(GL_BACK);

	glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer.FBO);
	glViewport(0, 0, m_framebuffer.colorTex.width, m_framebuffer.colorTex.height);
	glClearColor(1.f, 1.f, 1.f, 1.f);
	glClear(GL_COLOR_BUFFER_BIT);

	glBindTexture(GL_TEXTURE_2D, frambuffer.colorTex.texID);

	glUniform1f(glGetUniformLocation(m_program->m_shaderProgram, "uOffset"), 1.f / 300.f);
	glUniform1i(glGetUniformLocation(m_program->m_shaderProgram, "uTransformType"), (GLint)transformType);

	glBindVertexArray(gpuMesh->VAO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gpuMesh->EBO);
	glDrawElements(GL_TRIANGLES, (GLsizei)m_quadMesh->m_indices.size(), GL_UNSIGNED_INT, nullptr);

	glUseProgram(0);

	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// TODO: Make real swap
	std::swap(frambuffer.colorTex.texID, m_framebuffer.colorTex.texID);

	glBindFramebuffer(GL_FRAMEBUFFER, frambuffer.FBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, frambuffer.colorTex.texID, 0);

	glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer.FBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_framebuffer.colorTex.texID, 0);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

}
