#include <pch.hpp>

#include "prefilter_map_renderpass.hpp"

#include "framebuffer.hpp"
#include "sky_renderer.hpp"
#include "skydome_renderpass.hpp"
#include "texture.hpp"
#include "viewer.hpp"


PrefilterMapRenderPass::PrefilterMapRenderPass(const char* name)
// TODO: Set real path
	: ARenderingRenderPass(name, "Assets/Shaders/PBR/environmentMapShader.vert", "Assets/Shaders/PBR/prefilterMapShader.frag")
{
}

template <>
int PrefilterMapRenderPass::Subscribe(SkyRenderer* toGenerate)
{
	if (!toGenerate)
		return -1;

	if (!ElementMeshGenerator::Generate(toGenerate->m_cube.get()))
		return -1;

	m_skyRenderer = toGenerate;

	SetupPrefilterMap();

	return 1;
}

template <>
void PrefilterMapRenderPass::Unsubscribe(SkyRenderer* toGenerate)
{
	if (m_skyRenderer == toGenerate)
	{
		m_skyRenderer = nullptr;
		m_callExecute = nullptr;
	}
}

void PrefilterMapRenderPass::SetupPrefilterMap()
{
	if (!m_skyRenderer)
		return;

	Texture* spheremap = m_skyRenderer->m_texture.get();

	if (!spheremap || spheremap->GetWidth() <= 0 || spheremap->GetHeight() <= 0)
		return;

	if (!spheremap->m_gpuPrefilterMap)
	{
		if (!spheremap->m_gpuTextureCubemap)
			return;

		std::unique_ptr<GPUPrefilterMapSphereMap> gpuPrefilterMap = std::make_unique<GPUPrefilterMapSphereMap>(spheremap);
		gpuPrefilterMap->m_OnGpuReloaded = CCCallback::BindCallback(&PrefilterMapRenderPass::GeneratePrefilterMap, this);

		glUseProgram(m_program->m_shaderProgram);
		glUniform1i(glGetUniformLocation(m_program->m_shaderProgram, "environmentMap"), 0);

		spheremap->m_gpuPrefilterMap = std::move(gpuPrefilterMap);
		
		GeneratePrefilterMap();
	}

}

void PrefilterMapRenderPass::GeneratePrefilterMap()
{
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

	const Matrix4 captureProjectionMatrix = Matrix4::Perspective(CCMaths::DEG2RAD * 90.f, 1.0f, 0.1f, 10.f);
	const Matrix4 captureViews[] =
	{
	   Matrix4::LookAt({0.0f, 0.0f, 0.0f},	{-1.0f,  0.0f,  0.0f},	{0.0f, -1.0f,  0.0f}),
	   Matrix4::LookAt({0.0f, 0.0f, 0.0f},	{1.0f,  0.0f,  0.0f},	{0.0f, -1.0f,  0.0f}),
	   Matrix4::LookAt({0.0f, 0.0f, 0.0f},	{0.0f,  1.0f,  0.0f},	{0.0f,  0.0f,  1.0f}),
	   Matrix4::LookAt({0.0f, 0.0f, 0.0f},	{0.0f, -1.0f,  0.0f},	{0.0f,  0.0f, -1.0f}),
	   Matrix4::LookAt({0.0f, 0.0f, 0.0f},	{0.0f,  0.0f,  1.0f},	{0.0f, -1.0f,  0.0f}),
	   Matrix4::LookAt({0.0f, 0.0f, 0.0f},	{0.0f,  0.0f, -1.0f},	{0.0f, -1.0f,  0.0f})
	};


	glCullFace(GL_BACK);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(m_program->m_shaderProgram);
	glUniformMatrix4fv(glGetUniformLocation(m_program->m_shaderProgram, "uProjection"), 1, GL_FALSE, captureProjectionMatrix.data);

	Texture* spheremap = m_skyRenderer->m_texture.get();
	Mesh* mesh = m_skyRenderer->m_cube.get();

	auto gpuCubemap = static_cast<SkyRenderer::GPUSkybox*>(spheremap->m_gpuTextureCubemap.get());
	auto gpuPrefilterMap = static_cast<GPUPrefilterMapSphereMap*>(spheremap->m_gpuPrefilterMap.get());
	auto gpuMesh = static_cast<GPUMeshBasic*>(mesh->m_gpuMesh.get());

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, gpuCubemap->ID);

	GLuint FBO;
	GLuint RBO;

	glGenFramebuffers(1, &FBO);
	glGenRenderbuffers(1, &RBO);

	glBindFramebuffer(GL_FRAMEBUFFER, FBO);

	for (unsigned int mip = 0; mip < gpuPrefilterMap->m_maxMipLevels; ++mip)
	{
		unsigned int mipWidth = static_cast<unsigned int>(gpuPrefilterMap->m_mipMapResolution * std::powf(0.5, (float)mip));
		unsigned int mipHeight = static_cast<unsigned int>(gpuPrefilterMap->m_mipMapResolution * std::powf(0.5, (float)mip));

		glBindRenderbuffer(GL_RENDERBUFFER, RBO);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, mipWidth, mipHeight);
		glViewport(0, 0, mipWidth, mipHeight);

		float roughness = (float)mip / (float)(gpuPrefilterMap->m_maxMipLevels - 1);
		glUniform1f(glGetUniformLocation(m_program->m_shaderProgram, "roughness"), roughness);

		for (unsigned int i = 0; i < 6; ++i)
		{
			glUniformMatrix4fv(glGetUniformLocation(m_program->m_shaderProgram, "uView"), 1, GL_FALSE, captureViews[i].data);

			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
				GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, gpuPrefilterMap->ID, mip);

			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			glBindVertexArray(gpuMesh->VAO);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gpuMesh->EBO);
			glDrawElements(GL_TRIANGLES, gpuMesh->indicesCount, GL_UNSIGNED_INT, nullptr);
		}
	}

	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glDeleteRenderbuffers(1, &RBO);
	glDeleteFramebuffers(1, &FBO);
}

void PrefilterMapRenderPass::GPUPrefilterMapSphereMap::Generate(Texture* texture)
{
	//--- Generate environment map --//
	glCreateTextures(GL_TEXTURE_CUBE_MAP, 1, &ID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, ID);

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); // to combatting visible dots artifact in the reflectance
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	for (unsigned int i = 0; i < 6; ++i)
	{
		// note that we store each face with 16 bit floating point values
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F,
			m_mipMapResolution, m_mipMapResolution, 0, GL_RGB, GL_FLOAT, nullptr);
	}

	glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

	texture->ClearData();

	if (m_OnGpuReloaded)
		m_OnGpuReloaded->Invoke();
}

void PrefilterMapRenderPass::GPUPrefilterMapSphereMap::Regenerate(Texture* texture)
{
	Destroy();

	if (texture->GetSurface() == ETextureSurface::TEXTURE_SPHEREMAP)
		Generate(texture);
}

void PrefilterMapRenderPass::GPUPrefilterMapSphereMap::Destroy()
{
	if (m_OnReloaded)
		m_OnReloaded->Unbind(&GPUPrefilterMapSphereMap::OnReload, this);
	glDeleteTextures(1, &ID);
}

PrefilterMapRenderPass::GPUPrefilterMapSphereMap::GPUPrefilterMapSphereMap(Texture* texture)
{
	m_OnReloaded = &texture->m_OnReloaded;
	m_OnReloaded->Bind(&GPUPrefilterMapSphereMap::OnReload, this);
	Generate(texture);
}

PrefilterMapRenderPass::GPUPrefilterMapSphereMap::~GPUPrefilterMapSphereMap()
{
	Destroy();
}

void PrefilterMapRenderPass::GPUPrefilterMapSphereMap::OnReload(std::shared_ptr<Texture> texture)
{
	Regenerate(texture.get());
}
