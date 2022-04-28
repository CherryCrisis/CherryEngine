#include "pch.hpp"

#include "prefilter_map_renderpass.hpp"

#include "skydome.hpp"
#include "skydome_renderpass.hpp"
#include "environment_map_renderpass.hpp"
#include "viewer.hpp"
#include "framebuffer.hpp"

PrefilterMapRenderPass::PrefilterMapRenderPass(const char* name)
// TODO: Set real path
	: ARenderingRenderPass(name, "Assets/pbrShaders/environmentMapShader.vert", "Assets/pbrShaders/prefilterMapShader.frag")
{
}

template <>
int PrefilterMapRenderPass::Subscribe(Skydome* toGenerate)
{
	if (!toGenerate)
		return -1;

	Spheremap* spheremap = toGenerate->m_spheremap.get();

	if (!spheremap || spheremap->GetWidth() <= 0 || spheremap->GetHeight() <= 0 || !spheremap->m_gpuSpheremap)
		return -1;

	if (!spheremap->m_gpuPrefilterMap)
	{
		std::unique_ptr<GPUPrefilterMapSphereMap> gpuPrefilterMap = std::make_unique<GPUPrefilterMapSphereMap>();

		//--- Generate environment map --//
		glCreateTextures(GL_TEXTURE_CUBE_MAP, 1, &gpuPrefilterMap->ID);
		glBindTexture(GL_TEXTURE_CUBE_MAP, gpuPrefilterMap->ID);

		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		for (unsigned int i = 0; i < 6; ++i)
		{
			// note that we store each face with 16 bit floating point values
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F,
				m_mipMapResolution, m_mipMapResolution, 0, GL_RGB, GL_FLOAT, nullptr);
		}

		glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
		glUseProgram(m_program->m_shaderProgram);
		glUniform1i(glGetUniformLocation(m_program->m_shaderProgram, "environmentMap"), 0);

		spheremap->m_gpuPrefilterMap = std::move(gpuPrefilterMap);

		if (!ElementMeshGenerator::Generate(toGenerate->m_mesh.get()))
			return -1;
	}

	m_skydome = toGenerate;

	if (m_skydome && m_program)
		m_callExecute = CCCallback::BindCallback(&PrefilterMapRenderPass::Execute, this);

	return 1;
}

template <>
void PrefilterMapRenderPass::Unsubscribe(Skydome* toGenerate)
{
	if (m_skydome == toGenerate)
	{
		m_skydome = nullptr;
		m_callExecute = nullptr;
	}
}

void PrefilterMapRenderPass::Execute(Framebuffer& fb, Viewer*& viewer)
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

	Spheremap* skyTexture = m_skydome->m_spheremap.get();
	Mesh* mesh = m_skydome->m_mesh.get();

	auto gpuSpheremap = static_cast<EnvironmentMapRenderPass::GPUSkydomeSpheremap*>(skyTexture->m_gpuSpheremap.get());
	auto gpuCubemap = static_cast<SkydomeRenderPass::GPUSkydomeCubemap*>(skyTexture->m_gpuCubemapV2.get());
	auto gpuPrefilterMap = static_cast<GPUPrefilterMapSphereMap*>(skyTexture->m_gpuPrefilterMap.get());
	auto gpuMesh = static_cast<GPUMeshBasic*>(mesh->m_gpuMesh.get());
	//auto gpuIrradianceMap = static_cast<IrradianceMapRenderPass::GPUIrradianceMapSphereMap*>(skyTexture->m_gpuIrradiancemap.get());

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, gpuCubemap->ID);

	glBindFramebuffer(GL_FRAMEBUFFER, gpuSpheremap->FBO);

	for (unsigned int mip = 0; mip < m_maxMipLevels; ++mip)
	{
		unsigned int mipWidth = m_mipMapResolution * std::pow(0.5, mip);
		unsigned int mipHeight = m_mipMapResolution * std::pow(0.5, mip);

		glBindRenderbuffer(GL_RENDERBUFFER, gpuSpheremap->RBO);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, mipWidth, mipHeight);
		glViewport(0, 0, mipWidth, mipHeight);

		float roughness = (float)mip / (float)(m_maxMipLevels - 1);
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

	glViewport(0, 0, fb.colorTex.width, fb.colorTex.height);


}
