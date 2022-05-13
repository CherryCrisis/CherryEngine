#include "pch.hpp"

#include "irradiance_map_renderpass.hpp"

#include "sky_renderer.hpp"
#include "skydome_renderpass.hpp"
#include "environment_map_renderpass.hpp"
#include "viewer.hpp"
#include "framebuffer.hpp"

IrradianceMapRenderPass::IrradianceMapRenderPass(const char* name)
// TODO: Set real path
	: ARenderingRenderPass(name, "Assets/Shaders/PBR/environmentMapShader.vert", "Assets/Shaders/PBR/irradianceMapShader.frag")
{
}

template <>
int IrradianceMapRenderPass::Subscribe(SkyRenderer* toGenerate)
{
	if (!toGenerate)
		return -1;

	if (!ElementMeshGenerator::Generate(toGenerate->m_cube.get()))
		return -1;

	m_skyRenderer = toGenerate;

	SetupIrradianceMap();

	return 1;
}

template <>
void IrradianceMapRenderPass::Unsubscribe(SkyRenderer* toGenerate)
{
	if (m_skyRenderer == toGenerate)
	{
		m_skyRenderer = nullptr;
		m_callExecute = nullptr;
	}
}

void IrradianceMapRenderPass::SetupIrradianceMap()
{
	if (!m_skyRenderer)
		return;

	Texture* spheremap = m_skyRenderer->m_texture.get();

	if (!spheremap || spheremap->GetWidth() <= 0 || spheremap->GetHeight() <= 0 || !spheremap->m_gpuTextureSpheremap)
		return;

	if (!spheremap->m_gpuIrradiancemap)
	{
		std::unique_ptr<GPUIrradianceMapSphereMap> gpuIrradianceMap = std::make_unique<GPUIrradianceMapSphereMap>();

		spheremap->m_gpuIrradiancemap = std::move(gpuIrradianceMap);

	}
}

void IrradianceMapRenderPass::GenerateIrradianceMap()
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

	auto gpuSpheremap = static_cast<EnvironmentMapRenderPass::GPUEnvironmentMap*>(spheremap->m_gpuTextureSpheremap.get());
	auto gpuCubemap = static_cast<SkydomeRenderPass::GPUSkydomeCubemap*>(spheremap->m_gpuTextureCubemap.get());
	auto gpuIrradianceMap = static_cast<GPUIrradianceMapSphereMap*>(spheremap->m_gpuIrradiancemap.get());
	auto gpuMesh = static_cast<GPUMeshBasic*>(mesh->m_gpuMesh.get());

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, gpuCubemap->ID);

	glViewport(0, 0, gpuIrradianceMap->mapSize, gpuIrradianceMap->mapSize);
	glBindFramebuffer(GL_FRAMEBUFFER, gpuSpheremap->FBO);

	for (unsigned int i = 0; i < 6; ++i)
	{
		glUniformMatrix4fv(glGetUniformLocation(m_program->m_shaderProgram, "uView"), 1, GL_FALSE, captureViews[i].data);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
			GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, gpuIrradianceMap->ID, 0);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glBindVertexArray(gpuMesh->VAO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gpuMesh->EBO);
		glDrawElements(GL_TRIANGLES, gpuMesh->indicesCount, GL_UNSIGNED_INT, nullptr);
	}

	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

}

void IrradianceMapRenderPass::GPUIrradianceMapSphereMap::Generate(Texture* texture)
{
	//--- Generate environment map --//
	glCreateTextures(GL_TEXTURE_CUBE_MAP, 1, &ID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, ID);

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	for (unsigned int i = 0; i < 6; ++i)
	{
		// note that we store each face with 16 bit floating point values
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F,
			mapSize, mapSize, 0, GL_RGB, GL_FLOAT, nullptr);
	}

	texture->ClearData();

	if (m_OnGpuReloaded)
		m_OnGpuReloaded->Invoke();
}

void IrradianceMapRenderPass::GPUIrradianceMapSphereMap::Regenerate(Texture* texture)
{
	Destroy();
	Generate(texture);
}

void IrradianceMapRenderPass::GPUIrradianceMapSphereMap::Destroy()
{
	glDeleteTextures(1, &ID);
}

IrradianceMapRenderPass::GPUIrradianceMapSphereMap::GPUIrradianceMapSphereMap(Texture* texture)
{
	m_OnTextureReloaded = &texture->m_OnReloaded;
	m_OnTextureReloaded->Bind(&GPUIrradianceMapSphereMap::OnReload, this);

	Generate(texture);
}

IrradianceMapRenderPass::GPUIrradianceMapSphereMap::~GPUIrradianceMapSphereMap()
{
	m_OnTextureReloaded->Unbind(&GPUIrradianceMapSphereMap::OnReload, this);
	Destroy();
}

void IrradianceMapRenderPass::GPUIrradianceMapSphereMap::OnReload(std::shared_ptr<Texture> texture)
{
	Regenerate(texture.get());
}
