#include <pch.hpp>

#include "skybox_renderpass.hpp"

#include "framebuffer.hpp"
#include "texture.hpp"
#include "viewer.hpp"


SkyboxRenderPass::SkyboxRenderPass(const char* name)
	: ARenderingRenderPass(name, "Assets/Shaders/LIT/skyboxShader.vert", "Assets/Shaders/LIT/skyboxShader.frag")
{
}

template <>
int SkyboxRenderPass::Subscribe(SkyRenderer* toGenerate)
{
	if (!toGenerate)
		return -1;

	Texture* cubemap = toGenerate->m_texture.get();
	
	if (!cubemap || cubemap->GetWidth() <= 0 || cubemap->GetHeight() <= 0)
		return -1;
	
	//if (!m_meshGenerator.Generate(toGenerate->m_cube.get()))
	//	return -1;

	if (!ElementMeshGenerator::Generate(toGenerate->m_cube.get()))
		return -1;

	if (!cubemap->m_gpuTextureCubemap)
	{
		if (!cubemap->GetData())
		{
			Resource<Texture>::ReloadResource(toGenerate->m_texture.get(), true, false);

			if (!cubemap->GetData())
				return -1;
		}
		
		std::unique_ptr<GPUSkyboxCubemap> gpuCubemap = std::make_unique<GPUSkyboxCubemap>(cubemap);
		cubemap->m_gpuTextureCubemap = std::move(gpuCubemap);
	}
	
	m_skyRenderer = toGenerate;

	if (m_skyRenderer && m_program)
		m_callExecute = CCCallback::BindCallback(&SkyboxRenderPass::Execute, this);
	
	return 1;
}

template <>
void SkyboxRenderPass::Unsubscribe(SkyRenderer* toGenerate)
{
	if (m_skyRenderer == toGenerate)
	{
		m_skyRenderer = nullptr;
		m_callExecute = nullptr;
	}
}

void SkyboxRenderPass::Execute(Viewer* viewer, Framebuffer* framebuffer)
{
	if (!viewer)
		return;

	
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer->FBO);
	
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glCullFace(GL_FRONT);
	
	glDepthFunc(GL_LEQUAL);
	
	glUseProgram(m_program->m_shaderProgram);
	
	CCMaths::Matrix4 viewProjection = viewer->m_projectionMatrix * viewer->m_viewMatrix;
	glUniformMatrix4fv(glGetUniformLocation(m_program->m_shaderProgram, "uViewProjection"), 1, GL_FALSE, viewProjection.data);
	
	Mesh* mesh = m_skyRenderer->m_cube.get();
	
	auto gpuMesh = static_cast<ElementMeshGenerator::GPUMeshBasic*>(mesh->m_gpuMesh.get());
	glBindVertexArray(gpuMesh->VAO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gpuMesh->EBO);
	Texture* cubemap = m_skyRenderer->m_texture.get();
	
	if (auto gpuCubemap = static_cast<GPUSkyboxCubemap*>(cubemap->m_gpuTextureCubemap.get()))
		glBindTextureUnit(0, gpuCubemap->ID);

	glDrawElements(GL_TRIANGLES, gpuMesh->indicesCount, GL_UNSIGNED_INT, nullptr);
	
	glUseProgram(0);
	
	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void SkyboxRenderPass::GPUSkyboxCubemap::Generate(Texture* texture)
{
	unsigned char* data = static_cast<unsigned char*>(texture->GetData());

	int faceSize = texture->GetHeight() / 6;
	int faceDataSize = texture->GetSize() / 6;

	glCreateTextures(GL_TEXTURE_CUBE_MAP, 1, &ID);
	glTextureStorage2D(ID, 1, GL_RGB8, faceSize, faceSize);

	glTextureParameteri(ID, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTextureParameteri(ID, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTextureParameteri(ID, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTextureParameteri(ID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTextureParameteri(ID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	for (int faceID = 0; faceID < 6; faceID++)
	{
		glTextureSubImage3D(ID, 0, 0, 0, faceID, faceSize, faceSize, 1, GL_BGR, GL_UNSIGNED_BYTE, data + static_cast<int64_t>(faceDataSize) * static_cast<int64_t>(faceID));
	}

	if (m_OnGpuReloaded)
		m_OnGpuReloaded->Invoke();
}

void SkyboxRenderPass::GPUSkyboxCubemap::Regenerate(Texture* texture)
{
	Destroy();

	if (texture->GetSurface() == ETextureSurface::TEXTURE_CUBEMAP)
		Generate(texture);
}

void SkyboxRenderPass::GPUSkyboxCubemap::Destroy()
{
	glDeleteTextures(1, &ID);
}

SkyboxRenderPass::GPUSkyboxCubemap::GPUSkyboxCubemap(Texture* texture)
{
	m_OnTextureReloaded = &texture->m_OnReloaded;
	m_OnTextureReloaded->Bind(&GPUSkyboxCubemap::OnReload, this);

	Generate(texture);
}

SkyboxRenderPass::GPUSkyboxCubemap::~GPUSkyboxCubemap()
{
	m_OnTextureReloaded->Unbind(&GPUSkyboxCubemap::OnReload, this);
	Destroy();
}

void SkyboxRenderPass::GPUSkyboxCubemap::OnReload(Texture* texture)
{
	Regenerate(texture);
}
