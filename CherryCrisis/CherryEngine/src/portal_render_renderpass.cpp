#include "pch.hpp"

#include "portal_render_renderpass.hpp"

#include "framebuffer.hpp"

#include "camera.hpp"
#include "model_renderer.hpp"
#include "transform.hpp"
#include "model.hpp"

#include "shadow_renderpass.hpp"
#include "portal.hpp"
#include "viewer.hpp"

PortalRenderPass::PortalRenderPass(const char* name)
	: ARenderingRenderPass(name, "Assets/portalShader.vert", "Assets/portalShader.frag")
{
	if (m_program)
		m_callExecute = CCCallback::BindCallback(&PortalRenderPass::Execute, this);

	m_quadMesh = ResourceManager::GetInstance()->AddResourceRef<Mesh>("CC_NormalizedQuad", true);

	if (!m_quadMesh->m_gpuMesh)
	{
		Mesh::CreateQuad(m_quadMesh, 1.f, 1.f);
		m_meshGenerator.Generate(m_quadMesh.get());
	}
}

template <>
int PortalRenderPass::Subscribe(Portal* toGenerate)
{
	if (!m_portalGenerator.Generate(toGenerate))
		return -1;
	
	m_portals.insert(toGenerate);
	return 1;
}

template <>
void PortalRenderPass::Unsubscribe(Portal* toGenerate)
{
	m_portals.erase(toGenerate);
}

void PortalRenderPass::Execute(Framebuffer& framebuffer, Viewer*& viewer)
{
	if (!viewer)
		return;

	if (viewer->m_currentIteration > 0)
	for (Portal* portal : m_portals)
	{
		if (GPUBasicPortal* gpuPortal = static_cast<GPUBasicPortal*>(portal->m_gpuPortal.get()))
		{
			portal->m_projectionMatrix = viewer->m_projectionMatrix;
			portal->Draw(gpuPortal->framebuffer, viewer->m_currentIteration - 1);
		}
	}

	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer.FBO);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glDepthFunc(GL_LESS);

	glCullFace(GL_BACK);

	glClearColor(0.f, 0.f, 0.f, 1.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(m_program->m_shaderProgram);
	CCMaths::Matrix4 viewProjection = viewer->m_projectionMatrix * viewer->m_viewMatrix;
	glUniformMatrix4fv(glGetUniformLocation(m_program->m_shaderProgram, "uViewProjection"), 1, GL_FALSE, viewProjection.data);

	auto gpuMesh = static_cast<ElementMeshGenerator::GPUMeshBasic*>(m_quadMesh->m_gpuMesh.get());

	if (!gpuMesh)
		return;

	glBindVertexArray(gpuMesh->VAO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gpuMesh->EBO);

	for (Portal* portal : m_portals)
	{
		GPUBasicPortal* gpuPortal = static_cast<GPUBasicPortal*>(portal->m_gpuPortal.get());

		if (!gpuPortal)
			continue;

		glUniformMatrix4fv(glGetUniformLocation(m_program->m_shaderProgram, "uModel"), 1, GL_FALSE, portal->m_modelMatrix.data);

		glBindTextureUnit(0, gpuPortal->framebuffer.colorTex.texID);

		glDrawElements(GL_TRIANGLES, gpuMesh->indicesCount, GL_UNSIGNED_INT, nullptr);
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	glUseProgram(0);
}