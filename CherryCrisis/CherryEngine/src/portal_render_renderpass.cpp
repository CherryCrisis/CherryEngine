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
		Generate(m_quadMesh.get());
	}
}

template <>
int PortalRenderPass::Subscribe(Portal* toGenerate)
{
	if (!toGenerate)
		return -1;

	if (toGenerate->m_gpuPortal)
	{
		m_portals.insert(toGenerate);
		return 1;
	}

	auto gpuPortal = std::make_unique<GPUBasicPortal>();

	gpuPortal->framebuffer.colorTex.width = 1000;
	gpuPortal->framebuffer.colorTex.height = 1000;

	glGenFramebuffers(1, &gpuPortal->framebuffer.FBO);
	glBindFramebuffer(1, gpuPortal->framebuffer.FBO);
	
	glGenTextures(1, &gpuPortal->framebuffer.colorTex.texID);
	glBindTexture(GL_TEXTURE_2D, gpuPortal->framebuffer.colorTex.texID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, gpuPortal->framebuffer.colorTex.width, gpuPortal->framebuffer.colorTex.height, 0, GL_RGBA, GL_FLOAT, NULL);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

	// Attach texture to framebuffer
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gpuPortal->framebuffer.colorTex.texID, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	toGenerate->m_gpuPortal = std::move(gpuPortal);

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

	for (Portal* portal : m_portals)
	{
		portal->m_projectionMatrix = viewer->m_projectionMatrix;

		if (GPUBasicPortal* gpuPortal = static_cast<GPUBasicPortal*>(portal->m_gpuPortal.get()))
			portal->Draw(gpuPortal->framebuffer);
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

	GPUMeshBasic* gpuMesh = static_cast<GPUMeshBasic*>(m_quadMesh->m_gpuMesh.get());

	if (!gpuMesh)
		return;

	glBindVertexArray(gpuMesh->VAO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gpuMesh->EBO);

	for (Portal* portal : m_portals)
	{
		CCMaths::Matrix4 model = Matrix4::Translate(portal->m_position) * Matrix4::RotateZYX(portal->m_rotation);

		glUniformMatrix4fv(glGetUniformLocation(m_program->m_shaderProgram, "uModel"), 1, GL_FALSE, model.data);

		GPUBasicPortal* gpuPortal = static_cast<GPUBasicPortal*>(portal->m_gpuPortal.get());

		if (!gpuPortal)
			continue;

		glBindTextureUnit(0, gpuPortal->framebuffer.colorTex.texID);

		glDrawElements(GL_TRIANGLES, gpuMesh->indicesCount, GL_UNSIGNED_INT, nullptr);
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	glUseProgram(0);
}
