#include <pch.hpp>

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

	m_framebuffer.Init();
}

void BasicPostProcessRenderPass::Execute(Framebuffer& framebuffer)
{
	//UpdateFramebuffer verif if width and height are changed
	m_framebuffer.UpdateFramebuffer(static_cast<float>(framebuffer.width), static_cast<float>(framebuffer.height));

	auto gpuMesh = static_cast<ElementMeshGenerator::GPUMeshBasic*>(m_quadMesh->m_gpuMesh.get());

	if (!gpuMesh)
		return;

	glUseProgram(m_program->m_shaderProgram);

	glDisable(GL_DEPTH_TEST);
	glCullFace(GL_BACK);

	glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer.FBO);
	glViewport(0, 0, m_framebuffer.width, m_framebuffer.height);

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
						   0, 0, m_framebuffer.width, m_framebuffer.height,
						   0, 0, framebuffer.width, framebuffer.height,
						   GL_COLOR_BUFFER_BIT, GL_NEAREST);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
