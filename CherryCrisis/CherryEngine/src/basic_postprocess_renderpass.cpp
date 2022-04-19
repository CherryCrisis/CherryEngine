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

	m_framebuffer.width = 1920; m_framebuffer.height = 1080;

	// TODO: Use DSA
	// TODO: Optimize
	glGenFramebuffers(1, &m_framebuffer.FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer.FBO);

	glGenTextures(1, &m_framebuffer.TexID);
	glBindTexture(GL_TEXTURE_2D, m_framebuffer.TexID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_framebuffer.width, m_framebuffer.height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_framebuffer.TexID, 0);

	glGenRenderbuffers(1, &m_framebuffer.RBO);
	glBindRenderbuffer(GL_RENDERBUFFER, m_framebuffer.RBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, m_framebuffer.width, m_framebuffer.height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_framebuffer.RBO);

	glBindTexture(GL_TEXTURE_2D, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
}

void BasicPostProcessRenderPass::Execute(Framebuffer& frambuffer)
{
	if (frambuffer.width != m_framebuffer.width ||
		frambuffer.height != m_framebuffer.height)
	{
		// TODO: Set one renderpass for each cam to avoid switching at each frame
		m_framebuffer.width = frambuffer.width; m_framebuffer.height = frambuffer.height;

		glBindRenderbuffer(GL_RENDERBUFFER, m_framebuffer.RBO);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, m_framebuffer.width, m_framebuffer.height);

		glBindTexture(GL_TEXTURE_2D, m_framebuffer.TexID);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_framebuffer.width, m_framebuffer.height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);

		glBindTexture(GL_TEXTURE_2D, 0);
		glBindRenderbuffer(GL_RENDERBUFFER, 0);
	}

	auto gpuMesh = static_cast<GPUMeshBasic*>(m_quadMesh->m_gpuMesh.get());

	if (!gpuMesh)
		return;

	glUseProgram(m_program->m_shaderProgram);

	glDisable(GL_DEPTH_TEST);
	glCullFace(GL_BACK);

	glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer.FBO);
	glViewport(0, 0, m_framebuffer.width, m_framebuffer.height);
	glClearColor(1.f, 1.f, 1.f, 1.f);
	glClear(GL_COLOR_BUFFER_BIT);

	glBindTexture(GL_TEXTURE_2D, frambuffer.TexID);

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
	std::swap(frambuffer, m_framebuffer);
}
