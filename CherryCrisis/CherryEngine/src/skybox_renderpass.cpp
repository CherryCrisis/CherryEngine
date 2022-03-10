#include "pch.hpp"

#include "skybox_renderpass.hpp"

#include "skybox.hpp"
#include "camera_component.hpp"

SkyboxRenderPass::SkyboxRenderPass(const char* name)
// TODO: Set real path
	: ElementMeshRenderPass(name, "Assets/skyboxShader.vert", "Assets/skyboxShader.frag")
{

}

template <>
int SkyboxRenderPass::Generate(Skybox* toGenerate)
{
	if (!toGenerate)
		return -1;

	Cubemap* cubemap = toGenerate->m_cubemap.get();

	if (!cubemap)
		return -1;

	GPUSkyboxCubemap gpuCubemap;

	glCreateTextures(GL_TEXTURE_CUBE_MAP, 1, &gpuCubemap.ID);
	glTextureStorage2D(gpuCubemap.ID, 1, GL_RGBA8, cubemap->GetWidth(), cubemap->GetHeight());

	glTextureParameteri(gpuCubemap.ID, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTextureParameteri(gpuCubemap.ID, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTextureParameteri(gpuCubemap.ID, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTextureParameteri(gpuCubemap.ID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTextureParameteri(gpuCubemap.ID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	if (cubemap->GetWidth() <= 0 || cubemap->GetHeight() <= 0)
		return -1;

	for (int faceID = 0; faceID < 6; faceID++)
	{
		if (!cubemap->GetData()[faceID])
			return -1;

		glTextureSubImage3D(gpuCubemap.ID, 0, 0, 0, faceID, cubemap->GetWidth(), cubemap->GetHeight(), 1, GL_RGBA, GL_UNSIGNED_BYTE, cubemap->GetData()[faceID]);
	}

	cubemap->m_gpuCubemap = new GPUSkyboxCubemap(gpuCubemap);

	if (ElementMeshRenderPass::Generate(toGenerate->m_mesh.get()) == -1)
		return -1;

	m_skybox = toGenerate;

	return 1;
}

template <>
void SkyboxRenderPass::Remove(Skybox* toGenerate)
{
	if (m_skybox == toGenerate)
		m_skybox = nullptr;
}

void SkyboxRenderPass::Execute()
{
	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);

	glDepthFunc(GL_LEQUAL);

	glUseProgram(m_program->m_shaderProgram);

	if (!m_skybox)
		return;

	if (m_cameraComp)
	{
		CCMaths::Matrix4 projection = Matrix4::Perspective(m_cameraComp->m_camera.fovY, m_cameraComp->m_camera.aspect, m_cameraComp->m_camera.near, m_cameraComp->m_camera.far);
		CCMaths::Matrix4 view = Matrix4::RotateYXZ(-m_cameraComp->m_transform->GetRotation());

		CCMaths::Matrix4 viewProjection = projection * view;
		glUniformMatrix4fv(glGetUniformLocation(m_program->m_shaderProgram, "uViewProjection"), 1, GL_FALSE, viewProjection.data);
	}

	Mesh* mesh = m_skybox->m_mesh.get();

	if (!mesh)
		return;

	if (GPUMeshBasic* gpuMesh = static_cast<GPUMeshBasic*>(mesh->m_gpuMesh))
	{
		glBindVertexArray(gpuMesh->VAO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gpuMesh->EBO);
		glDrawElements(GL_TRIANGLES, mesh->m_indices.size(), GL_UNSIGNED_INT, nullptr);
	}

	if (Cubemap* skyTexture = m_skybox->m_cubemap.get())
	{
		if (auto gpuCubemap = static_cast<GPUSkyboxCubemap*>(skyTexture->m_gpuCubemap))
			glBindTextureUnit(0, gpuCubemap->ID);
	}

	glUseProgram(0);

	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

template <>
int SkyboxRenderPass::Generate(CameraComponent* toGenerate)
{
	if (!toGenerate)
		return -1;

	m_cameraComp = toGenerate;
}

template <>
void SkyboxRenderPass::Remove(CameraComponent* toGenerate)
{
	m_cameraComp = nullptr;
}