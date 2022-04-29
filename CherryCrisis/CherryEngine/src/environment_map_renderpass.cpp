#include "pch.hpp"

#include "environment_map_renderpass.hpp"

#include "skydome.hpp"
#include "skydome_renderpass.hpp"
#include "viewer.hpp"
#include "framebuffer.hpp"

EnvironmentMapRenderPass::EnvironmentMapRenderPass(const char* name)
// TODO: Set real path
	: ARenderingRenderPass(name, "Assets/pbrShaders/environmentMapShader.vert", "Assets/pbrShaders/environmentMapShader.frag")
{
}

template <>
int EnvironmentMapRenderPass::Subscribe(Skydome* toGenerate)
{
	if (!toGenerate)
		return -1;

	Spheremap* spheremap = toGenerate->m_spheremap.get();

	if (!spheremap || spheremap->GetWidth() <= 0 || spheremap->GetHeight() <= 0)
		return -1;

	if (!spheremap->m_gpuSpheremap)
	{
		std::unique_ptr<GPUSkydomeSpheremap> gpuSpheremap = std::make_unique<GPUSkydomeSpheremap>();

		//--- Generate environment map --//
		glCreateTextures(GL_TEXTURE_2D, 1, &gpuSpheremap->ID);

		glTextureParameteri(gpuSpheremap->ID, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTextureParameteri(gpuSpheremap->ID, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTextureParameteri(gpuSpheremap->ID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTextureParameteri(gpuSpheremap->ID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		if (!spheremap->GetData())
			return -1;

		//TODO: Integer Compressed spheremap
		glTextureStorage2D(gpuSpheremap->ID, 1, GL_RGB8, spheremap->GetWidth(), spheremap->GetHeight());
		glTextureSubImage2D(gpuSpheremap->ID, 0, 0, 0, spheremap->GetWidth(), spheremap->GetHeight(), (GLenum)spheremap->GetInternalFormat(), GL_UNSIGNED_BYTE, spheremap->GetData());

		//--- Generate FBO and RBO  to convert sphereMap in Cubemap--//
		glGenFramebuffers(1, &gpuSpheremap->FBO);
		glGenRenderbuffers(1, &gpuSpheremap->RBO);

		glBindFramebuffer(GL_FRAMEBUFFER, gpuSpheremap->FBO);
		glBindRenderbuffer(GL_RENDERBUFFER, gpuSpheremap->RBO);

		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, width, height);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, gpuSpheremap->RBO);

		glUseProgram(m_program->m_shaderProgram);
		glUniform1i(glGetUniformLocation(m_program->m_shaderProgram, "equirectangularMap"), 0);

		spheremap->m_gpuSpheremap = std::move(gpuSpheremap);

		if (!ElementMeshGenerator::Generate(toGenerate->m_mesh.get()))
			return -1;
	}

	m_skydome = toGenerate;
	toGenerate->ClearData();

	if (m_skydome && m_program)
		m_callExecute = CCCallback::BindCallback(&EnvironmentMapRenderPass::Execute, this);

	return 1;
}

template <>
void EnvironmentMapRenderPass::Unsubscribe(Skydome* toGenerate)
{
	if (m_skydome == toGenerate)
	{
		m_skydome = nullptr;
		m_callExecute = nullptr;
	}
}

void EnvironmentMapRenderPass::Execute(Framebuffer& fb, Viewer*& viewer)
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

		Spheremap* skyTexture = m_skydome->m_spheremap.get();
		GPUSkydomeSpheremap* gpuSpheremap = static_cast<GPUSkydomeSpheremap*>(skyTexture->m_gpuSpheremap.get());
		SkydomeRenderPass::GPUSkydomeCubemap* gpuCubemap = static_cast<SkydomeRenderPass::GPUSkydomeCubemap*>(skyTexture->m_gpuCubemapV2.get());

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, gpuSpheremap->ID);

		glViewport(0, 0, width, height); // don't forget to configure the viewport to the capture dimensions.
		glBindFramebuffer(GL_FRAMEBUFFER, gpuSpheremap->FBO);

		Mesh* mesh = m_skydome->m_mesh.get();
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

		glViewport(0, 0, fb.colorTex.width, fb.colorTex.height);
	}
}
