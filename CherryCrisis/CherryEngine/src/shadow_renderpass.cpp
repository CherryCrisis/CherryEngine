#include "pch.hpp"

#include "shadow_renderpass.hpp"
#include "element_mesh_generator.hpp"

#include "model_renderer.hpp"
#include "light.hpp"

#include "transform.hpp"
#include "model.hpp"
#include "mesh.hpp"

ShadowRenderPass::GPUShadowLight::~GPUShadowLight()
{
	if (depthTexID)
		glDeleteTextures(1, &depthTexID);

	if (framebuffer.FBO)
		glDeleteFramebuffers(1, &framebuffer.FBO);
}

ShadowRenderPass::ShadowRenderPass(const char* name)
	: ARenderingRenderPass(name, "Assets/shadowShader.vert", "Assets/shadowShader.frag")
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

	auto gpuLight = std::make_unique<GPUShadowLight>();

	gpuLight->framebuffer.width = 1000;
	gpuLight->framebuffer.height = 1000;

	glGenFramebuffers(1, &gpuLight->framebuffer.FBO);
	glGenTextures(1, &gpuLight->depthTexID);
	glBindTexture(GL_TEXTURE_2D, gpuLight->depthTexID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, gpuLight->framebuffer.width, gpuLight->framebuffer.height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

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
int ShadowRenderPass::Subscribe(ModelRenderer* toGenerate)
{
	if (!toGenerate->m_model)
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
void ShadowRenderPass::Unsubscribe(Light* toGenerate)
{
	m_lights.erase(toGenerate);
}

void ShadowRenderPass::Execute(Framebuffer& framebuffer, Camera& camera)
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

		glViewport(0, 0, gpuLight->framebuffer.width, gpuLight->framebuffer.height);
		glBindFramebuffer(GL_FRAMEBUFFER, gpuLight->framebuffer.FBO);
		glClear(GL_DEPTH_BUFFER_BIT);

		// TODO: Optimize, compute one time
		CCMaths::Matrix4 lightView = CCMaths::Matrix4::LookAt(light->m_position);
		CCMaths::Matrix4 ortho = CCMaths::Matrix4::Transpose(CCMaths::Matrix4::Orthographic(-10.f, 10.f, -10.f, 10.f, -50.f, 20.f));
		CCMaths::Matrix4 lightSpace = ortho * lightView;

		for (ModelRenderer* modelRdr : m_models)
		{
			if (!modelRdr->m_isVisible)
				continue;

			CCMaths::Matrix4 lightSpaceModel = lightSpace * modelRdr->m_transform->GetWorldMatrix();
			glUniformMatrix4fv(glGetUniformLocation(m_program->m_shaderProgram, "uLightSpaceModel"), 1, GL_FALSE, lightSpaceModel.data);

			Model* model = modelRdr->m_model.get();

			if (!model)
				continue;

			Mesh* mesh = model->m_mesh.get();

			if (!mesh)
				continue;

			auto* gpuMesh = static_cast<ElementMeshGenerator::GPUMeshBasic*>(mesh->m_gpuMesh.get());

			if (!gpuMesh)
				continue;

			glBindVertexArray(gpuMesh->VAO);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gpuMesh->EBO);
			glDrawElements(GL_TRIANGLES, (GLsizei)mesh->m_indices.size(), GL_UNSIGNED_INT, nullptr);
		}
	}

	glViewport(0, 0, framebuffer.width, framebuffer.height);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer.FBO);

	glCullFace(GL_BACK);

	glUseProgram(0);

	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}