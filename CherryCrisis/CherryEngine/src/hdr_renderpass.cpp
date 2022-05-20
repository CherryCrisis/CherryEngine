#include "pch.hpp"

#include "hdr_renderpass.hpp"

#include "mesh.hpp"
#include "resource_manager.hpp"

HDRRenderPass::HDRRenderPass(const char* name)
	: APostProcessRenderPass(name, "Assets/Shaders/LIT/hdrShader.vert", "Assets/Shaders/LIT/hdrShader.frag")
{
	if (m_program)
	{
		glUseProgram(m_program->m_shaderProgram);
		glUniform1i(glGetUniformLocation(m_program->m_shaderProgram, "uHdrBuffer"), 0);
		glUniform1i(glGetUniformLocation(m_program->m_shaderProgram, "uBloomBlur"), 1);

		m_callExecute = CCCallback::BindCallback(&HDRRenderPass::Execute, this);
	}

	m_quadMesh = ResourceManager::GetInstance()->AddResourceRef<Mesh>("CC_NormalizedQuad", true);

	if (!m_quadMesh->m_gpuMesh)
	{
		Mesh::CreateQuad(m_quadMesh, 1.f, 1.f);
		m_meshGenerator.Generate(m_quadMesh.get());
	}

	m_framebuffer.Init();
}

void HDRRenderPass::Execute(Framebuffer& framebuffer)
{
	//UpdateFramebuffer verif if width and height are changed
	m_framebuffer.UpdateFramebuffer(framebuffer.width, framebuffer.height);

	auto gpuMesh = static_cast<ElementMeshGenerator::GPUMeshBasic*>(m_quadMesh->m_gpuMesh.get());

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

	glDrawElements(GL_TRIANGLES, gpuMesh->indicesCount, GL_UNSIGNED_INT, nullptr);

	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glUseProgram(0);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glBlitNamedFramebuffer(m_framebuffer.FBO, framebuffer.FBO,
						   0, 0, m_framebuffer.width, m_framebuffer.height,
						   0, 0, framebuffer.width, framebuffer.height,
						   GL_COLOR_BUFFER_BIT, GL_NEAREST);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
