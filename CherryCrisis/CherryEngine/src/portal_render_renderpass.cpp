#include "pch.hpp"

#include "portal_render_renderpass.hpp"

#include "camera.hpp"
#include "framebuffer.hpp"
#include "mixed_rendering_pipeline.hpp"
#include "model_renderer.hpp"
#include "portal.hpp"
#include "shadow_renderpass.hpp"
#include "transform.hpp"
#include "viewer.hpp"


PortalRenderPass::PortalRenderPass(const char* name)
	: ARenderingRenderPass(name, "Assets/Shaders/portalShader.vert", "Assets/Shaders/portalShader.frag")
{
	if (m_program)
		m_callExecute = CCCallback::BindCallback(&PortalRenderPass::Execute, this);

	m_quadMesh = ResourceManager::GetInstance()->AddResourceRef<Mesh>("CC_EuclideanPortal", true);

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

void PortalRenderPass::Execute(Viewer*& viewer)
{
	if (!viewer)
		return;

	const Framebuffer& framebuffer = *viewer->m_framebuffer;

	if (viewer->m_currentIteration > 0)
		for (Portal* portal : m_portals)
		{
			if (!portal->m_linkedPortal)
				continue;

			if (PortalGenerator::GPUBasicPortal* gpuPortal = static_cast<PortalGenerator::GPUBasicPortal*>(portal->m_gpuPortal.get()))
			{
				if (!gpuPortal->framebuffer)
					continue;

				if (!viewer->m_frustumPlanes.IsOnFrustum(portal->m_modelMatrix, m_quadMesh->m_aabb))
					continue;

				//UpdateFrameBuffer verif if width and heigth are changed
				gpuPortal->framebuffer->UpdateFramebuffer(static_cast<float>(framebuffer.width), static_cast<float>(framebuffer.height));

				portal->m_linkedPortal->m_framebuffer = gpuPortal->framebuffer;

				//Portal view world matrix from linked portal
				//CCMaths::Matrix4 portalViewModel = portal->m_linkedPortal->m_modelMatrix * portal->m_modelMatrix.Inverse() * viewer->m_viewMatrix.Inverse();
				CCMaths::Matrix4 portalViewModel = portal->m_relativeLinkedPortalMatrix * viewer->m_viewMatrix.Inverse();

				Vector3 TRS[3];
				Matrix4::Decompose(portalViewModel, TRS[0], TRS[1], TRS[2]);

				//Remove scale from portalViewModel and make portal view matrix
				CCMaths::Matrix4 portalView = Matrix4::RotateXYZ(-TRS[1]) * Matrix4::Translate(-TRS[0]);

				portal->m_linkedPortal->m_viewMatrix = portalView;

				//Necessary for the PBR
				portal->m_linkedPortal->m_position = portalViewModel.position;

				const CCMaths::Matrix4& clipPlaneWorldMatrix = portal->m_linkedPortal->m_modelMatrix;
				const int dot = CCMaths::Sign(Vector3::Dot(clipPlaneWorldMatrix.back, clipPlaneWorldMatrix.position - portalViewModel.position));

				Vector3 viewSpacePos = (portalView * Vector4(clipPlaneWorldMatrix.position, 1.f)).xyz;

				portalView.position = Vector3::Zero; //To only rotate clipPlaneWorldMatrix
				Vector3 viewSpaceNormal = (portalView * Vector4(clipPlaneWorldMatrix.back, 1.f)).xyz * dot;

				float viewSpaceDist = -Vector3::Dot(viewSpacePos, viewSpaceNormal);

				Vector4 clipPlaneViewSpace = Vector4(viewSpaceNormal.x, viewSpaceNormal.y, viewSpaceNormal.z, viewSpaceDist);

				//Make clipping in front of the portal
				portal->m_linkedPortal->m_projectionMatrix = Matrix4::ObliqueProjection(clipPlaneViewSpace, viewer->m_projectionMatrix);

				float aspect = (float)framebuffer.width / (float)framebuffer.height;
				portal->m_linkedPortal->m_frustumPlanes = FrustumPlanes::CreateFromInverse(portalViewModel, portal->m_fovY, aspect, portal->m_near, portal->m_far);

				portal->m_linkedPortal->Draw(viewer->m_currentIteration - 1);
			}
		}

	glViewport(0, 0, framebuffer.width, framebuffer.height);

	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer.FBO);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glDisable(GL_CULL_FACE);

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
		PortalGenerator::GPUBasicPortal* gpuPortal = static_cast<PortalGenerator::GPUBasicPortal*>(portal->m_gpuPortal.get());

		if (!gpuPortal)
			continue;

		if (!viewer->m_frustumPlanes.IsOnFrustum(portal->m_modelMatrix, m_quadMesh->m_aabb))
			continue;

		glBindTextureUnit(0, gpuPortal->framebuffer->colorTex.texID);

		glUniformMatrix4fv(glGetUniformLocation(m_program->m_shaderProgram, "uModel"), 1, GL_FALSE, portal->m_modelMatrix.data);

		glDrawElements(GL_TRIANGLES, gpuMesh->indicesCount, GL_UNSIGNED_INT, nullptr);
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	glUseProgram(0);
}
