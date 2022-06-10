#include <pch.hpp>

#include "brdf_renderpass.hpp"

#include "environment_map_renderpass.hpp"
#include "framebuffer.hpp"
#include "viewer.hpp"


BRDFRenderPass::BRDFRenderPass(const char* name)
	: ARenderingRenderPass(name, "Assets/Shaders/PBR/brdfShader.vert", "Assets/Shaders/PBR/brdfShader.frag")
{
}

template <>
int BRDFRenderPass::Subscribe(SkyRenderer* toGenerate)
{
	if (!toGenerate)
		return -1;

	if (!ElementMeshGenerator::Generate(toGenerate->m_quad.get()))
		return -1;

	m_skyRenderer = toGenerate;

	if (!toGenerate->m_gpuBRDF)
	{
		std::unique_ptr<GPUBRDFSphereMap> gpuBrdf = std::make_unique<GPUBRDFSphereMap>();

		//--- Generate environment map --//
		glCreateTextures(GL_TEXTURE_2D, 1, &gpuBrdf->ID);
		glBindTexture(GL_TEXTURE_2D, gpuBrdf->ID);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		// pre-allocate enough memory for the LUT texture.
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RG16F, gpuBrdf->resolution, gpuBrdf->resolution, 0, GL_RG, GL_FLOAT, 0);

		toGenerate->m_gpuBRDF = std::move(gpuBrdf);

		GenerateBRDF();
	}
	
	return 1;
}

template <>
void BRDFRenderPass::Unsubscribe(SkyRenderer* toGenerate)
{
	if (m_skyRenderer == toGenerate)
	{
		m_skyRenderer = nullptr;
		m_callExecute = nullptr;
	}
}

void BRDFRenderPass::GenerateBRDF()
{
	glCullFace(GL_BACK);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(m_program->m_shaderProgram);

	Mesh* quad = m_skyRenderer->m_quad.get();

	auto gpuBrdf = static_cast<GPUBRDFSphereMap*>(m_skyRenderer->m_gpuBRDF.get());
	auto gpuMesh = static_cast<GPUMeshBasic*>(quad->m_gpuMesh.get());

	glViewport(0, 0, gpuBrdf->resolution, gpuBrdf->resolution); 

	GLuint FBO;
	GLuint RBO;

	glGenFramebuffers(1, &FBO);
	glGenRenderbuffers(1, &RBO);

	glBindFramebuffer(GL_FRAMEBUFFER, FBO);
	glBindRenderbuffer(GL_RENDERBUFFER, RBO);

	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, gpuBrdf->resolution, gpuBrdf->resolution);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gpuBrdf->ID, 0);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//Draw quad
	glBindVertexArray(gpuMesh->VAO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gpuMesh->EBO);
	glDrawElements(GL_TRIANGLES, gpuMesh->indicesCount, GL_UNSIGNED_INT, nullptr);

	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glDeleteRenderbuffers(1, &RBO);
	glDeleteFramebuffers(1, &FBO);

}
