#include "pch.hpp"

#include "skydome_renderpass.hpp"

#include "skydome.hpp"
#include "viewer.hpp"
#include "framebuffer.hpp"

SkydomeRenderPass::SkydomeRenderPass(const char* name)
// TODO: Set real path
	: ARenderingRenderPass(name, "Assets/Shaders/PBR/skydomeShader.vert", "Assets/Shaders/PBR/skydomeShader.frag")
{
}

template <>
int SkydomeRenderPass::Subscribe(Skydome* toGenerate)
{
	if (!toGenerate)
		return -1;

	Spheremap* spheremap = toGenerate->m_spheremap.get();

	if (!spheremap || spheremap->GetWidth() <= 0 || spheremap->GetHeight() <= 0)
		return -1;

	if (!spheremap->m_gpuCubemapV2)
	{
		std::unique_ptr<GPUSkydomeCubemap> gpuCubemap = std::make_unique<GPUSkydomeCubemap>();

		glCreateTextures(GL_TEXTURE_CUBE_MAP, 1, &gpuCubemap->ID);
		glBindTexture(GL_TEXTURE_CUBE_MAP, gpuCubemap->ID);

		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		
		for (unsigned int i = 0; i < 6; ++i)
		{
			// note that we store each face with 16 bit floating point values
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F,
				1024, 1024, 0, GL_RGB, GL_FLOAT, nullptr);
		}

		glUseProgram(m_program->m_shaderProgram);
		glUniform1i(glGetUniformLocation(m_program->m_shaderProgram, "environmentMap"), 0);

		glBindTexture(GL_TEXTURE_CUBE_MAP, 0);


		//TODO: Integer Compressed spheremap
		
		spheremap->m_gpuCubemapV2 = std::move(gpuCubemap);

		if (!ElementMeshGenerator::Generate(toGenerate->m_cube.get()))
			return -1;
	}

	m_skydome = toGenerate;
	toGenerate->ClearData();

	if (m_skydome && m_program)
		m_callExecute = CCCallback::BindCallback(&SkydomeRenderPass::Execute, this);

	return 1;
}

template <>
void SkydomeRenderPass::Unsubscribe(Skydome* toGenerate)
{
	if (m_skydome == toGenerate)
	{
		m_skydome = nullptr;
		m_callExecute = nullptr;
	}
}

void SkydomeRenderPass::Execute(Framebuffer& framebuffer, Viewer*& viewer)
{
	if (!viewer)
		return;

	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer.FBO);

	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glCullFace(GL_FRONT);

	glDepthFunc(GL_LEQUAL);

	glUseProgram(m_program->m_shaderProgram);

	glUniformMatrix4fv(glGetUniformLocation(m_program->m_shaderProgram, "uProjection"), 1, GL_FALSE, viewer->m_projectionMatrix.data);
	glUniformMatrix4fv(glGetUniformLocation(m_program->m_shaderProgram, "uView"), 1, GL_FALSE, viewer->m_viewMatrix.data);

	Mesh* mesh = m_skydome->m_cube.get();

	GPUMeshBasic* gpuMesh = static_cast<GPUMeshBasic*>(mesh->m_gpuMesh.get());
	glBindVertexArray(gpuMesh->VAO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gpuMesh->EBO);
	glDrawElements(GL_TRIANGLES, gpuMesh->indicesCount, GL_UNSIGNED_INT, nullptr);

	Spheremap* skyTexture = m_skydome->m_spheremap.get();
	GPUSkydomeCubemap* gpuCubemap = static_cast<GPUSkydomeCubemap*>(skyTexture->m_gpuCubemapV2.get());
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, gpuCubemap->ID);

	glUseProgram(0);

	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}
