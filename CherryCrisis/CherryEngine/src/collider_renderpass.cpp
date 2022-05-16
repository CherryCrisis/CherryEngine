#include "pch.hpp"

#include "collider_renderpass.hpp"

#include "framebuffer.hpp"

#include "camera.hpp"
#include "model_renderer.hpp"
#include "transform.hpp"
#include "mesh.hpp"

#include "box_collider.hpp"
#include "sphere_collider.hpp"
#include "capsule_collider.hpp"

#include "viewer.hpp"

ColliderRenderPass::ColliderRenderPass(const char* name)
	: ARenderingRenderPass(name, "Assets/Shaders/colliderShader.vert", "Assets/Shaders/colliderShader.frag")
{
	if (m_program)
	{
		glUseProgram(m_program->m_shaderProgram);

		m_callExecute = CCCallback::BindCallback(&ColliderRenderPass::Execute, this);

		glUseProgram(0);
	}
}

template <>
int ColliderRenderPass::Subscribe(Collider* toGenerate)
{
	if (!m_meshGenerator.Generate(toGenerate->GetMesh()))
		return -1;

	m_colliders.insert(toGenerate);

	return 1;
}

template <>
void ColliderRenderPass::Unsubscribe(Collider* toGenerate)
{
	m_colliders.erase(toGenerate);
}

void ColliderRenderPass::Execute(Framebuffer& framebuffer, Viewer*& viewer)
{
	if (!viewer)
		return;

	glViewport(0, 0, framebuffer.colorTex.width, framebuffer.colorTex.height);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer.FBO);

	glDisable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glDepthFunc(GL_LESS);

	glCullFace(GL_BACK);
	glPolygonMode(GL_FRONT, GL_LINE);

	glUseProgram(m_program->m_shaderProgram);

	CCMaths::Matrix4 viewProjection = viewer->m_projectionMatrix * viewer->m_viewMatrix;
	glUniformMatrix4fv(glGetUniformLocation(m_program->m_shaderProgram, "uViewProjection"), 1, GL_FALSE, viewProjection.data);

	for (Collider* collider : m_colliders)
	{
		if (!collider->m_isVisible)
			continue;

		CCMaths::Matrix4 modelMat = collider->GetTranformMatrix();
		glUniformMatrix4fv(glGetUniformLocation(m_program->m_shaderProgram, "uModel"), 1, GL_FALSE, modelMat.data);

		CCMaths::Vector4 color = { 0.f, 1.f, 0.f, 1.f };
		glUniform4fv(glGetUniformLocation(m_program->m_shaderProgram, "uColor"), 1, color.data);
		
		Mesh* mesh = collider->GetMesh();

		if (!mesh)
			continue;

		auto gpuMesh = static_cast<ElementTBNGenerator::GPUMeshBasic*>(mesh->m_gpuMesh.get());

		if (!gpuMesh)
			continue;

		glBindVertexArray(gpuMesh->VAO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gpuMesh->EBO);
		glDrawElements(GL_TRIANGLES, gpuMesh->indicesCount, GL_UNSIGNED_INT, nullptr);
	}

	glUseProgram(0);

	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}
