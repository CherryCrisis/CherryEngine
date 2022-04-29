#include "pch.hpp"

#include "brdf_renderpass.hpp"

#include "skydome.hpp"
#include "skydome_renderpass.hpp"
#include "environment_map_renderpass.hpp"
#include "viewer.hpp"
#include "framebuffer.hpp"

BRDFRenderPass::BRDFRenderPass(const char* name)
// TODO: Set real path
	: ARenderingRenderPass(name, "Assets/pbrShaders/brdfShader.vert", "Assets/pbrShaders/brdfShader.frag")
{
}

template <>
int BRDFRenderPass::Subscribe(Skydome* toGenerate)
{
	if (!toGenerate)
		return -1;

	Spheremap* spheremap = toGenerate->m_spheremap.get();

	if (!spheremap || spheremap->GetWidth() <= 0 || spheremap->GetHeight() <= 0 || !spheremap->m_gpuSpheremap)
		return -1;

	if (!spheremap->m_gpuBrdf)
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
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RG16F, m_resolution, m_resolution, 0, GL_RG, GL_FLOAT, 0);

		if (!ElementMeshGenerator::Generate(toGenerate->m_quad.get()))
			return -1;

		spheremap->m_gpuBrdf = std::move(gpuBrdf);

	}

	m_skydome = toGenerate;

	if (m_skydome && m_program)
		m_callExecute = CCCallback::BindCallback(&BRDFRenderPass::Execute, this);

	return 1;
}

template <>
void BRDFRenderPass::Unsubscribe(Skydome* toGenerate)
{
	if (m_skydome == toGenerate)
	{
		m_skydome = nullptr;
		m_callExecute = nullptr;
	}
}

void BRDFRenderPass::Execute(Framebuffer& fb, Viewer*& viewer)
{

	glCullFace(GL_BACK);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(m_program->m_shaderProgram);

	Spheremap* skyTexture = m_skydome->m_spheremap.get();
	Mesh* quad = m_skydome->m_quad.get();

	auto gpuSpheremap = static_cast<EnvironmentMapRenderPass::GPUSkydomeSpheremap*>(skyTexture->m_gpuSpheremap.get());
	auto gpuBrdf = static_cast<GPUBRDFSphereMap*>(skyTexture->m_gpuBrdf.get());
	auto gpuMesh = static_cast<GPUMeshBasic*>(quad->m_gpuMesh.get());

	glViewport(0, 0, m_resolution, m_resolution); // don't forget to configure the viewport to the capture dimensions.

	glBindFramebuffer(GL_FRAMEBUFFER, gpuSpheremap->FBO);
	glBindRenderbuffer(GL_RENDERBUFFER, gpuSpheremap->RBO);

	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, m_resolution, m_resolution);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gpuBrdf->ID, 0);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//Draw quad
	glBindVertexArray(gpuMesh->VAO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gpuMesh->EBO);
	glDrawElements(GL_TRIANGLES, gpuMesh->indicesCount, GL_UNSIGNED_INT, nullptr);

	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glViewport(0, 0, fb.colorTex.width, fb.colorTex.height);


}
