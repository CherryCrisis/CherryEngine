#include "pch.hpp"

#include "environment_map_renderpass.hpp"

#include "sky_renderer.hpp"
#include "skydome_renderpass.hpp"
#include "viewer.hpp"
#include "framebuffer.hpp"

EnvironmentMapRenderPass::EnvironmentMapRenderPass(const char* name)
// TODO: Set real path
	: ARenderingRenderPass(name, "Assets/Shaders/PBR/environmentMapShader.vert", "Assets/Shaders/PBR/environmentMapShader.frag")
{
}

template <>
int EnvironmentMapRenderPass::Subscribe(SkyRenderer* toGenerate)
{
	if (!toGenerate)
		return -1;

	if (!ElementMeshGenerator::Generate(toGenerate->m_cube.get()))
		return -1;

	m_skyRenderer = toGenerate;

	SetupEnvironmentMap();

	return 1;
}

template <>
void EnvironmentMapRenderPass::Unsubscribe(SkyRenderer* toGenerate)
{
	if (m_skyRenderer == toGenerate)
	{
		m_skyRenderer = nullptr;
		m_callExecute = nullptr;
	}
}

void EnvironmentMapRenderPass::SetupEnvironmentMap()
{
	if (!m_skyRenderer)
		return;

	Texture* spheremap = m_skyRenderer->m_texture.get();

	if (!spheremap || spheremap->GetWidth() <= 0 || spheremap->GetHeight() <= 0)
		return;

	if (!spheremap->m_gpuTextureSpheremap)
	{
		if (!spheremap->GetData())
		{
			Resource<Texture>::ReloadResource(m_skyRenderer->m_texture);

			if (!spheremap->GetData())
				return;
		}

		std::unique_ptr<GPUEnvironmentMap> gpuEnvMap = std::make_unique<GPUEnvironmentMap>(spheremap);
		gpuEnvMap->m_OnGpuReloaded = CCCallback::BindCallback(&EnvironmentMapRenderPass::GenerateEnvironmentMap, this);

		glUseProgram(m_program->m_shaderProgram);
		glUniform1i(glGetUniformLocation(m_program->m_shaderProgram, "equirectangularMap"), 0);

		spheremap->m_gpuTextureSpheremap = std::move(gpuEnvMap);
	}

	GenerateEnvironmentMap();
}

void EnvironmentMapRenderPass::GenerateEnvironmentMap()
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

	//Setup spheremap
	{
		glCullFace(GL_BACK);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// convert HDR equirectangular environment map to cubemap equivalent
		glUseProgram(m_program->m_shaderProgram);
		glUniformMatrix4fv(glGetUniformLocation(m_program->m_shaderProgram, "uProjection"), 1, GL_FALSE, captureProjectionMatrix.data);

		Texture* spheremap = m_skyRenderer->m_texture.get();
		GPUEnvironmentMap* gpuSpheremap = static_cast<GPUEnvironmentMap*>(spheremap->m_gpuTextureSpheremap.get());
		SkyRenderer::GPUSkybox* gpuCubemap = static_cast<SkyRenderer::GPUSkybox*>(spheremap->m_gpuTextureCubemap.get());

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, gpuSpheremap->ID);

		glViewport(0, 0, gpuSpheremap->envMapSize, gpuSpheremap->envMapSize);
		glBindFramebuffer(GL_FRAMEBUFFER, gpuSpheremap->FBO);

		Mesh* mesh = m_skyRenderer->m_cube.get();
		GPUMeshBasic* gpuMesh = static_cast<GPUMeshBasic*>(mesh->m_gpuMesh.get());

		for (unsigned int i = 0; i < 6; ++i)
		{

			glUniformMatrix4fv(glGetUniformLocation(m_program->m_shaderProgram, "uView"), 1, GL_FALSE, captureViews[i].data);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
				GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, gpuCubemap->ID, 0);

			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			glBindVertexArray(gpuMesh->VAO);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gpuMesh->EBO);
			glDrawElements(GL_TRIANGLES, gpuMesh->indicesCount, GL_UNSIGNED_INT, nullptr);
		}
		glBindVertexArray(0);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		glBindTexture(GL_TEXTURE_CUBE_MAP, gpuCubemap->ID);
		glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
	}
}

void EnvironmentMapRenderPass::GPUEnvironmentMap::Generate(Texture* texture)
{
	//--- Generate environment map --//
	glCreateTextures(GL_TEXTURE_2D, 1, &ID);

	glTextureParameteri(ID, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTextureParameteri(ID, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTextureParameteri(ID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTextureParameteri(ID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	//TODO: Integer Compressed spheremap
	glTextureStorage2D(ID, 1, GL_RGB16F, texture->GetWidth(), texture->GetHeight());
	glTextureSubImage2D(ID, 0, 0, 0, texture->GetWidth(), texture->GetHeight(), (GLenum)texture->GetInternalFormat(), GL_UNSIGNED_BYTE, texture->GetData());

	//--- Generate FBO and RBO  to convert sphereMap in Cubemap--//
	glGenFramebuffers(1, &FBO);
	glGenRenderbuffers(1, &RBO);

	glBindFramebuffer(GL_FRAMEBUFFER, FBO);
	glBindRenderbuffer(GL_RENDERBUFFER, RBO);

	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, envMapSize, envMapSize);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, RBO);

	texture->ClearData();

	if (m_OnGpuReloaded)
		m_OnGpuReloaded->Invoke();
}

void EnvironmentMapRenderPass::GPUEnvironmentMap::Regenerate(Texture* texture)
{
	Destroy();

	if (texture->GetSurface() == ETextureSurface::TEXTURE_SPHEREMAP)
		Generate(texture);
}

void EnvironmentMapRenderPass::GPUEnvironmentMap::Destroy()
{
	glDeleteTextures(1, &ID);
	glDeleteFramebuffers(1, &FBO);
	glDeleteRenderbuffers(1, &RBO);
}

EnvironmentMapRenderPass::GPUEnvironmentMap::GPUEnvironmentMap(Texture* texture)
{
	m_OnTextureReloaded = &texture->m_OnReloaded;
	m_OnTextureReloaded->Bind(&GPUEnvironmentMap::OnReload, this);

	Generate(texture);
}

EnvironmentMapRenderPass::GPUEnvironmentMap::~GPUEnvironmentMap()
{
	m_OnTextureReloaded->Unbind(&GPUEnvironmentMap::OnReload, this);
	Destroy();
}

void EnvironmentMapRenderPass::GPUEnvironmentMap::OnReload(std::shared_ptr<Texture> texture)
{
	Regenerate(texture.get());
}