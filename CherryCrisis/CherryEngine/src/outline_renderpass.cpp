#include <pch.hpp>

#include "outline_renderpass.hpp"

#include "audio_emitter.hpp"
#include "camera.hpp"
#include "camera_component.hpp"
#include "framebuffer.hpp"
#include "light_component.hpp"
#include "model.hpp"
#include "mesh_renderer.hpp"
#include "portal.hpp"
#include "shadow_renderpass.hpp"
#include "texture_generator.hpp"
#include "transform.hpp"
#include "viewer.hpp"


OutlineRenderPass::OutlineRenderPass(const char* name)
	: ARenderingRenderPass(name, "Assets/Shaders/outline.vert", "Assets/Shaders/outline.frag")
{
	if (m_program)
		m_callExecute = CCCallback::BindCallback(&OutlineRenderPass::Execute, this);
}

template <>
int OutlineRenderPass::Subscribe(MeshRenderer* toGenerate)
{
	m_meshRenderers.insert(toGenerate);
	return 1;
}
template <>
void OutlineRenderPass::Unsubscribe(MeshRenderer* toGenerate)
{
	m_meshRenderers.erase(toGenerate);
}

void OutlineRenderPass::Execute(Viewer*& viewer)
{
	if (!viewer)
		return;

	const Framebuffer& framebuffer = *viewer->m_framebuffer;
	glViewport(0, 0, framebuffer.width, framebuffer.height);

	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer.FBO);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glUseProgram(m_program->m_shaderProgram);
	
	CCMaths::Matrix4 viewMat = viewer->m_viewMatrix;
	CCMaths::Matrix4 projMat = viewer->m_projectionMatrix;

	glUniformMatrix4fv(glGetUniformLocation(m_program->m_shaderProgram, "uView"), 1, GL_FALSE, viewMat.data);
	glUniformMatrix4fv(glGetUniformLocation(m_program->m_shaderProgram, "uProjection"), 1, GL_FALSE, projMat.data);

	for (MeshRenderer* renderer : m_meshRenderers)
	{
		if (!renderer->m_isVisible)
			continue;

		Mesh* mesh = renderer->m_mesh.get();

		if (!mesh)
			continue;

		auto gpuMesh = static_cast<ElementTBNGenerator::GPUMeshBasic*>(renderer->m_mesh->m_gpuMesh.get());

		if (!gpuMesh)
			continue;

		CCMaths::Matrix4 modelMat = renderer->m_transform->GetWorldMatrix();

		if (!viewer->m_frustumPlanes.IsOnFrustum(modelMat, mesh->m_aabb))
			continue;

		CCMaths::Matrix4 normalMat = CCMaths::Matrix4::Transpose(modelMat.Inverse());

		glUniformMatrix4fv(glGetUniformLocation(m_program->m_shaderProgram, "uModel"), 1, GL_FALSE, modelMat.data);
		glUniformMatrix4fv(glGetUniformLocation(m_program->m_shaderProgram, "uNormalMatrix"), 1, GL_FALSE, normalMat.data);

		glBindVertexArray(gpuMesh->VAO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gpuMesh->EBO);
		glDrawElements(GL_TRIANGLES, gpuMesh->indicesCount, GL_UNSIGNED_INT, nullptr);
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	glUseProgram(0);
}
