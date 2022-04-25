#include "pch.hpp"

#include "hdr_renderpass.hpp"

#include "mesh.hpp"
#include "resource_manager.hpp"

HDRRenderPass::HDRRenderPass(const char* name)
	: APostProcessRenderPass(name, "Assets/hdrShader.vert", "Assets/hdrShader.frag")
{
	if (m_program)
	{
		glUseProgram(m_program->m_shaderProgram);
		glUniform1i(glGetUniformLocation(m_program->m_shaderProgram, "uHdrBuffer"), 0);
		glUniform1i(glGetUniformLocation(m_program->m_shaderProgram, "uBloomBlur"), 1);

		m_callExecute = CCCallback::BindCallback(&HDRRenderPass::Execute, this);
	}

	m_quadMesh = ResourceManager::GetInstance()->AddResourceRef<Mesh>("CC_NormalizedQuad", true);

	Mesh::CreateQuad(m_quadMesh, 1.f, 1.f);

	Generate(m_quadMesh.get());

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
}

void HDRRenderPass::Execute(Framebuffer& framebuffer)
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

	auto gpuMesh = static_cast<GPUMeshBasic*>(m_quadMesh->m_gpuMesh.get());

	if (!gpuMesh)
		return;

	glDisable(GL_DEPTH_TEST);
	glCullFace(GL_BACK);

	glUseProgram(m_program->m_shaderProgram);
	glUniform1f(glGetUniformLocation(m_program->m_shaderProgram, "uExposure"), 1.f);

	glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer.FBO);

	glBindVertexArray(gpuMesh->VAO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gpuMesh->EBO);

	glBindTextureUnit(0, framebuffer.colorTex.texID);
	glBindTextureUnit(1, inBrightness->texID);

	glDrawElements(GL_TRIANGLES, (GLsizei)m_quadMesh->m_indices.size(), GL_UNSIGNED_INT, nullptr);

	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glUseProgram(0);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glBlitNamedFramebuffer(m_framebuffer.FBO, framebuffer.FBO,
						   0, 0, m_framebuffer.colorTex.width, m_framebuffer.colorTex.height,
						   0, 0, framebuffer.colorTex.width, framebuffer.colorTex.height,
						   GL_COLOR_BUFFER_BIT, GL_NEAREST);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
