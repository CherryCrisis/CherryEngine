#include "pch.hpp"

#include "bloom_renderpass.hpp"

#include "mesh.hpp"
#include "resource_manager.hpp"

BloomRenderPass::BloomRenderPass(const char* name)
	: APostProcessRenderPass(name, "Assets/bloomShader.vert", "Assets/bloomShader.frag")
{
	if (m_hdrProgram = ResourceManager::GetInstance()->AddResource<ShaderProgram>("hdrProgram", true, "Assets/hdrShader.vert", "Assets/hdrShader.frag"))
	{
		glUseProgram(m_hdrProgram->m_shaderProgram);
		glUniform1i(glGetUniformLocation(m_hdrProgram->m_shaderProgram, "hdrBuffer"), 0);
		glUniform1i(glGetUniformLocation(m_hdrProgram->m_shaderProgram, "bloomBlur"), 1);

		if (m_program)
		{
			glUseProgram(m_program->m_shaderProgram);
			glUniform1i(glGetUniformLocation(m_program->m_shaderProgram, "brightImage"), 0);

			m_callExecute = CCCallback::BindCallback(&BloomRenderPass::Execute, this);
		}
	}


	m_quadMesh = ResourceManager::GetInstance()->AddResourceRef<Mesh>("CC_NormalizedQuad", true);

	Mesh::CreateQuad(m_quadMesh, 1.f, 1.f);

	Generate(m_quadMesh.get());

	for (int i = 0; i < 2; i++)
	{
		Framebuffer& framebuffer = m_pingpongFramebuffers[i];
		// TODO: Use DSA
		// TODO: Optimize
		glGenFramebuffers(1, &framebuffer.FBO);
		glBindFramebuffer(GL_FRAMEBUFFER, framebuffer.FBO);

		glGenTextures(1, &framebuffer.colorTex.texID);
		glBindTexture(GL_TEXTURE_2D, framebuffer.colorTex.texID);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, framebuffer.colorTex.width, framebuffer.colorTex.height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, framebuffer.colorTex.texID, 0);
	}

	glBindTexture(GL_TEXTURE_2D, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void BloomRenderPass::Execute(Framebuffer& frambuffer)
{
	for (int i = 0; i < 2; i++)
	{
		bool hasChanged = false;
		Framebuffer& framebuffer = m_pingpongFramebuffers[i];
		if (frambuffer.colorTex.width != framebuffer.colorTex.width || frambuffer.colorTex.height != framebuffer.colorTex.height)
		{
			framebuffer.colorTex.width = frambuffer.colorTex.width; framebuffer.colorTex.height = frambuffer.colorTex.height;

			glBindTexture(GL_TEXTURE_2D, framebuffer.colorTex.texID);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, framebuffer.colorTex.width, framebuffer.colorTex.height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);

			hasChanged = true;
		}

		if (hasChanged)
			glBindTexture(GL_TEXTURE_2D, 0);
	}

	auto gpuMesh = static_cast<GPUMeshBasic*>(m_quadMesh->m_gpuMesh.get());

	if (!gpuMesh)
		return;

	glUseProgram(m_program->m_shaderProgram);

	glBindVertexArray(gpuMesh->VAO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gpuMesh->EBO);

	bool horizontal = true, first_iteration = true;
	int amount = 11;
	for (unsigned int i = 0; i < amount; i++)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, m_pingpongFramebuffers[horizontal].FBO);
		glUniform1i(glGetUniformLocation(m_program->m_shaderProgram, "horizontal"), horizontal);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, first_iteration ? frambuffer.brightnessTex.texID : m_pingpongFramebuffers[!horizontal].colorTex.texID);

		glDrawElements(GL_TRIANGLES, (GLsizei)m_quadMesh->m_indices.size(), GL_UNSIGNED_INT, nullptr);

		horizontal = !horizontal;
		if (first_iteration)
			first_iteration = false;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, m_pingpongFramebuffers[horizontal].FBO);

	glClear(GL_COLOR_BUFFER_BIT);

	glUseProgram(m_hdrProgram->m_shaderProgram);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, frambuffer.colorTex.texID);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, m_pingpongFramebuffers[!horizontal].colorTex.texID);
	glActiveTexture(GL_TEXTURE0);

	glUniform1f(glGetUniformLocation(m_hdrProgram->m_shaderProgram, "exposure"), 0.5f);

	glDrawElements(GL_TRIANGLES, (GLsizei)m_quadMesh->m_indices.size(), GL_UNSIGNED_INT, nullptr);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glUseProgram(0);

	std::swap(frambuffer.colorTex, m_pingpongFramebuffers[horizontal].colorTex);

	glBindFramebuffer(GL_FRAMEBUFFER, frambuffer.FBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, frambuffer.colorTex.texID, 0);
	
	glBindFramebuffer(GL_FRAMEBUFFER, m_pingpongFramebuffers[horizontal].FBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_pingpongFramebuffers[horizontal].colorTex.texID, 0);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

}
