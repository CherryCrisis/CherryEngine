#include "pch.hpp"

#include "skybox_renderpass.hpp"

#include "skybox.hpp"
#include "viewer.hpp"
#include "framebuffer.hpp"

SkyboxRenderPass::SkyboxRenderPass(const char* name)
// TODO: Set real path
	: ARenderingRenderPass(name, "Assets/skyboxShader.vert", "Assets/skyboxShader.frag")
{
}

template <>
int SkyboxRenderPass::Subscribe(Skybox* toGenerate)
{
	if (!toGenerate)
		return -1;

	Cubemap* cubemap = toGenerate->m_cubemap.get();

	if (!cubemap)
		return -1;

	std::unique_ptr<GPUSkyboxCubemap> gpuCubemap = std::make_unique<GPUSkyboxCubemap>();

	glCreateTextures(GL_TEXTURE_CUBE_MAP, 1, &gpuCubemap->ID);
	glTextureStorage2D(gpuCubemap->ID, 1, GL_RGBA8, cubemap->GetWidth(), cubemap->GetHeight());

	glTextureParameteri(gpuCubemap->ID, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTextureParameteri(gpuCubemap->ID, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTextureParameteri(gpuCubemap->ID, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTextureParameteri(gpuCubemap->ID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTextureParameteri(gpuCubemap->ID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	if (cubemap->GetWidth() <= 0 || cubemap->GetHeight() <= 0)
		return -1;

	for (int faceID = 0; faceID < 6; faceID++)
	{
		if (!cubemap->GetData()[faceID])
			return -1;

		glTextureSubImage3D(gpuCubemap->ID, 0, 0, 0, faceID, cubemap->GetWidth(), cubemap->GetHeight(), 1, GL_BGR, GL_UNSIGNED_BYTE, cubemap->GetData()[faceID]);
	}

	cubemap->m_gpuCubemap = std::move(gpuCubemap);

	if (!ElementMeshGenerator::Generate(toGenerate->m_mesh.get()))
		return -1;

	m_skybox = toGenerate;

	if (m_skybox && m_program)
		m_callExecute = CCCallback::BindCallback(&SkyboxRenderPass::Execute, this);

	return 1;
}

template <>
void SkyboxRenderPass::Unsubscribe(Skybox* toGenerate)
{
	if (m_skybox == toGenerate)
	{
		m_skybox = nullptr;
		m_callExecute = nullptr;
	}
}

void SkyboxRenderPass::Execute(Framebuffer& framebuffer, Viewer*& viewer)
{
	if (!viewer)
		return;

	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);

	glDepthFunc(GL_LEQUAL);

	glUseProgram(m_program->m_shaderProgram);
	
	CCMaths::Matrix4 viewProjection = viewer->m_projectionMatrix * viewer->m_viewMatrix;
	glUniformMatrix4fv(glGetUniformLocation(m_program->m_shaderProgram, "uViewProjection"), 1, GL_FALSE, viewProjection.data);

	Mesh* mesh = m_skybox->m_mesh.get();

	GPUMeshBasic* gpuMesh = static_cast<GPUMeshBasic*>(mesh->m_gpuMesh.get());
	glBindVertexArray(gpuMesh->VAO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gpuMesh->EBO);
	glDrawElements(GL_TRIANGLES, (GLsizei)mesh->m_indices.size(), GL_UNSIGNED_INT, nullptr);

	Cubemap* skyTexture = m_skybox->m_cubemap.get();

	auto gpuCubemap = static_cast<GPUSkyboxCubemap*>(skyTexture->m_gpuCubemap.get());
	glBindTextureUnit(0, gpuCubemap->ID);

	glUseProgram(0);

	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}
