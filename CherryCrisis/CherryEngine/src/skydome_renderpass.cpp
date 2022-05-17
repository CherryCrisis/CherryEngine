#include "pch.hpp"

#include "skydome_renderpass.hpp"

#include "sky_renderer.hpp"
#include "viewer.hpp"
#include "framebuffer.hpp"

SkydomeRenderPass::SkydomeRenderPass(const char* name)
// TODO: Set real path
	: ARenderingRenderPass(name, "Assets/Shaders/PBR/skydomeShader.vert", "Assets/Shaders/PBR/skydomeShader.frag")
{
}

template <>
int SkydomeRenderPass::Subscribe(SkyRenderer* toGenerate)
{
	if (!toGenerate)
		return -1;

	m_skyRenderer = toGenerate;

	SetupSkydome();

	if (m_skyRenderer && m_program)
		m_callExecute = CCCallback::BindCallback(&SkydomeRenderPass::Execute, this);

	return 1;
}

template <>
void SkydomeRenderPass::Unsubscribe(SkyRenderer* toGenerate)
{
	if (m_skyRenderer == toGenerate)
	{
		m_skyRenderer = nullptr;
		m_callExecute = nullptr;
	}
}

void SkydomeRenderPass::SetupSkydome()
{
	if (!m_skyRenderer)
		return;

	Texture* spheremap = m_skyRenderer->m_texture.get();

	if (!spheremap || spheremap->GetWidth() <= 0 || spheremap->GetHeight() <= 0)
		return;

	if (!spheremap->m_gpuTextureCubemap)
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

		spheremap->m_gpuTextureCubemap = std::move(gpuCubemap);
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

	Texture* spheremap = m_skyRenderer->m_texture.get();
	GPUSkydomeCubemap* gpuCubemap = static_cast<GPUSkydomeCubemap*>(spheremap->m_gpuTextureCubemap.get());
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, gpuCubemap->ID);

	Mesh* mesh = m_skyRenderer->m_cube.get();
	GPUMeshBasic* gpuMesh = static_cast<GPUMeshBasic*>(mesh->m_gpuMesh.get());

	glBindVertexArray(gpuMesh->VAO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gpuMesh->EBO);
	glDrawElements(GL_TRIANGLES, gpuMesh->indicesCount, GL_UNSIGNED_INT, nullptr);


	glUseProgram(0);

	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
