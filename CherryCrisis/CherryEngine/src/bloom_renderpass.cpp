#include "pch.hpp"

#include "bloom_renderpass.hpp"

#include "mesh.hpp"
#include "resource_manager.hpp"

BloomRenderPass::BloomRenderPass(const char* name)
	: APostProcessRenderPass(name, "Assets/bloomShader.vert", "Assets/bloomShader.frag")
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

	for (int i = 0; i < 2; i++)
	{
		Framebuffer& framebuffer = m_pingpongFramebuffers[i];
		// TODO: Use DSA
		// TODO: Optimize
		glGenFramebuffers(1, &framebuffer.FBO);
		glBindFramebuffer(GL_FRAMEBUFFER, framebuffer.FBO);

		glGenTextures(1, &framebuffer.colorTex.texID);
		glBindTexture(GL_TEXTURE_2D, framebuffer.colorTex.texID);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, framebuffer.colorTex.width, framebuffer.colorTex.height, 0, GL_RGB, GL_FLOAT, NULL);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, framebuffer.colorTex.texID, 0);
	}

	glBindTexture(GL_TEXTURE_2D, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void BloomRenderPass::Execute(Framebuffer& framebuffer)
{
	for (int i = 0; i < 2; i++)
	{
		bool hasChanged = false;
		Framebuffer& curFramebuffer = m_pingpongFramebuffers[i];
		if (framebuffer.colorTex.width != curFramebuffer.colorTex.width || framebuffer.colorTex.height != curFramebuffer.colorTex.height)
		{
			curFramebuffer.colorTex.width = framebuffer.colorTex.width; curFramebuffer.colorTex.height = framebuffer.colorTex.height;

			glBindTexture(GL_TEXTURE_2D, curFramebuffer.colorTex.texID);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, curFramebuffer.colorTex.width, curFramebuffer.colorTex.height, 0, GL_RGB, GL_FLOAT, NULL);

			hasChanged = true;
		}

		if (hasChanged)
			glBindTexture(GL_TEXTURE_2D, 0);
	}

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
