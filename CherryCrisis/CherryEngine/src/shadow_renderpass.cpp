#include "pch.hpp"

#include "shadow_renderpass.hpp"
#include "element_mesh_generator.hpp"


#include "transform.hpp"
#include "mesh.hpp"
#include "model_renderer.hpp"
#include "shape_renderer.hpp"
#include "viewer.hpp"

ShadowRenderPass::GPUShadowLight::~GPUShadowLight()
{
	if (depthTexID)
		glDeleteTextures(1, &depthTexID);

	if (framebuffer.FBO)
		glDeleteFramebuffers(1, &framebuffer.FBO);
}

ShadowRenderPass::ShadowRenderPass(const char* name)
	: ARenderingRenderPass(name, "Assets/Shaders/shadowShader.vert", "Assets/Shaders/shadowShader.frag")
{
	if (m_program)
	{
		glUseProgram(m_program->m_shaderProgram);

		glUniform1i(glGetUniformLocation(m_program->m_shaderProgram, "uMaterial.albedoTex"), 0);
		glUniform1i(glGetUniformLocation(m_program->m_shaderProgram, "uMaterial.normalMap"), 1);

		m_callExecute = CCCallback::BindCallback(&ShadowRenderPass::Execute, this);

		glUseProgram(0);
	}
}

template <>
int ShadowRenderPass::Subscribe(Light* toGenerate)
{
	if (!toGenerate)
		return -1;

	if (toGenerate->m_gpuLight)
	{
		m_lights.insert(toGenerate);
		return true;
	}

	auto gpuLight = std::make_unique<GPUShadowLight>();

	gpuLight->framebuffer.colorTex.width = 1000;
	gpuLight->framebuffer.colorTex.height = 1000;

	// TODO: Use DSA

	glGenFramebuffers(1, &gpuLight->framebuffer.FBO);
	glGenTextures(1, &gpuLight->depthTexID);
	glBindTexture(GL_TEXTURE_2D, gpuLight->depthTexID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, gpuLight->framebuffer.colorTex.width, gpuLight->framebuffer.colorTex.height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	// Clamp to border to deactivate frustum shadows
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

	float borderColor[] = { 1.f, 1.f, 1.f, 1.f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

	// Attach texture to framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, gpuLight->framebuffer.FBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, gpuLight->depthTexID, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	toGenerate->m_gpuLight = std::move(gpuLight);

	m_lights.insert(toGenerate);

	return 1;
}

template <>
void ShadowRenderPass::Unsubscribe(Light* toGenerate)
{
	m_lights.erase(toGenerate);
}

template <>
int ShadowRenderPass::Subscribe(ModelRenderer* toGenerate)
{
	if (!toGenerate->m_mesh)
		return -1;

	if (!m_meshGenerator.Generate(toGenerate->m_mesh.get()))
		return -1;

	m_models.insert(toGenerate);

	return 1;
}

template <>
void ShadowRenderPass::Unsubscribe(ModelRenderer* toGenerate)
{
	m_models.erase(toGenerate);
}

template <>
int ShadowRenderPass::Subscribe(ShapeRenderer* toGenerate)
{
	if (!toGenerate->m_mesh)
		return -1;

	if (!m_meshGenerator.Generate(toGenerate->m_mesh.get()))
		return -1;

	m_shapes.insert(toGenerate);

	return 1;
}

template <>
void ShadowRenderPass::Unsubscribe(ShapeRenderer* toGenerate)
{
	m_shapes.erase(toGenerate);
}

void ShadowRenderPass::Execute(Framebuffer& framebuffer, Viewer*& viewer)
{
	glEnable(GL_DEPTH_TEST);
	glCullFace(GL_BACK);

	glUseProgram(m_program->m_shaderProgram);

	glClearColor(0.f, 0.f, 0.f, 1.f);

	for (Light* light : m_lights)
	{
		auto* gpuLight = static_cast<GPUShadowLight*>(light->m_gpuLight.get());

		if (!gpuLight)
			continue;

		glViewport(0, 0, gpuLight->framebuffer.colorTex.width, gpuLight->framebuffer.colorTex.height);
		glBindFramebuffer(GL_FRAMEBUFFER, gpuLight->framebuffer.FBO);
		glClear(GL_DEPTH_BUFFER_BIT);

		for (ModelRenderer* modelRdr : m_models)
		{
			if (!modelRdr->m_isVisible)
				continue;

			CCMaths::Matrix4 lightSpaceModel = light->m_lightSpace * modelRdr->m_transform->GetWorldMatrix();
			glUniformMatrix4fv(glGetUniformLocation(m_program->m_shaderProgram, "uLightSpaceModel"), 1, GL_FALSE, lightSpaceModel.data);

			Mesh* mesh = modelRdr->m_mesh.get();

			if (!mesh)
				continue;

			auto* gpuMesh = static_cast<ElementMeshGenerator::GPUMeshBasic*>(mesh->m_gpuMesh.get());

			if (!gpuMesh)
				continue;

			glBindVertexArray(gpuMesh->VAO);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gpuMesh->EBO);
			glDrawElements(GL_TRIANGLES, gpuMesh->indicesCount, GL_UNSIGNED_INT, nullptr);
		}
	}

	glUseProgram(0);

	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}