#include "pch.hpp"

#include "bloom_renderpass.hpp"

#include "mesh.hpp"
#include "resource_manager.hpp"

BloomRenderPass::BloomRenderPass(const char* name)
	: APostProcessRenderPass(name, "Assets/Shaders/bloomShader.vert", "Assets/Shaders/bloomShader.frag")
{
	if (m_program)
	{
		glUseProgram(m_program->m_shaderProgram);
		glUniform1i(glGetUniformLocation(m_program->m_shaderProgram, "brightImage"), 0);

		m_callExecute = CCCallback::BindCallback(&BloomRenderPass::Execute, this);
	}

	m_quadMesh = ResourceManager::GetInstance()->AddResourceRef<Mesh>("CC_NormalizedQuad", true);

	if (!m_quadMesh->m_gpuMesh)
	{
		Mesh::CreateQuad(m_quadMesh, 1.f, 1.f);
		m_meshGenerator.Generate(m_quadMesh.get());
	}

	m_pingpongFramebuffers[0].Init();
	m_pingpongFramebuffers[1].Init();
}

void BloomRenderPass::Execute(Framebuffer& framebuffer)
{
	//UpdateFramebuffer verif if width and height are changed
	m_pingpongFramebuffers[0].UpdateFramebuffer(framebuffer.width, framebuffer.height);
	m_pingpongFramebuffers[1].UpdateFramebuffer(framebuffer.width, framebuffer.height);

	auto gpuMesh = static_cast<ElementMeshGenerator::GPUMeshBasic*>(m_quadMesh->m_gpuMesh.get());

	if (!gpuMesh)
		return;

	glDisable(GL_DEPTH_TEST);
	glCullFace(GL_BACK);

	glUseProgram(m_program->m_shaderProgram);

	glBindVertexArray(gpuMesh->VAO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gpuMesh->EBO);

	bool horizontal = true, first_iteration = true;
	// TODO: Change this all
	unsigned int amount = 11u;
	for (unsigned int i = 0; i < amount; i++)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, m_pingpongFramebuffers[horizontal].FBO);
		glUniform1i(glGetUniformLocation(m_program->m_shaderProgram, "horizontal"), horizontal);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, first_iteration ? framebuffer.brightnessTex.texID : m_pingpongFramebuffers[!horizontal].colorTex.texID);

		glDrawElements(GL_TRIANGLES, gpuMesh->indicesCount, GL_UNSIGNED_INT, nullptr);

		horizontal = !horizontal;
		if (first_iteration)
			first_iteration = false;
	}

	outBrightness = &m_pingpongFramebuffers[horizontal].colorTex;

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
