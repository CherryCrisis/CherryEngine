#include <pch.hpp>

#include "shadow_renderpass.hpp"

#include "element_mesh_generator.hpp"
#include "mesh.hpp"
#include "model_renderer.hpp"
#include "shape_renderer.hpp"
#include "transform.hpp"
#include "viewer.hpp"

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

	if (!m_lightGenerator.Generate(toGenerate))
		return -1;

	m_lights.insert(toGenerate);

	return 1;
}

template <>
void ShadowRenderPass::Unsubscribe(Light* toGenerate)
{
	m_lights.erase(toGenerate);
}

template <>
int ShadowRenderPass::Subscribe(MeshRenderer* toGenerate)
{
	if (!toGenerate->m_mesh)
		return -1;

	if (!m_meshGenerator.Generate(toGenerate->m_mesh.get()))
		return -1;

	m_models.insert(toGenerate);

	return 1;
}

template <>
void ShadowRenderPass::Unsubscribe(MeshRenderer* toGenerate)
{
	m_models.erase(toGenerate);
}

void ShadowRenderPass::Execute(Viewer*& viewer)
{
	glEnable(GL_DEPTH_TEST);
	glCullFace(GL_BACK);

	glUseProgram(m_program->m_shaderProgram);

	glClearColor(0.f, 0.f, 0.f, 1.f);

	for (Light* light : m_lights)
	{
		auto* gpuLight = static_cast<LightGenerator::GPULightBasic*>(light->m_gpuLight.get());

		if (!gpuLight)
			continue;

		glViewport(0, 0, gpuLight->framebuffer.width, gpuLight->framebuffer.height);
		glBindFramebuffer(GL_FRAMEBUFFER, gpuLight->framebuffer.FBO);
		glClear(GL_DEPTH_BUFFER_BIT);

		for (MeshRenderer* modelRdr : m_models)
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