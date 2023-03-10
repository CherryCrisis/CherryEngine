#include "pch.hpp"

#include "portal_render_renderpass.hpp"

#include <limits>

#include "camera.hpp"
#include "framebuffer.hpp"
#include "mixed_rendering_pipeline.hpp"
#include "model_renderer.hpp"
#include "portal.hpp"
#include "shadow_renderpass.hpp"
#include "transform.hpp"
#include "viewer.hpp"

std::array<Framebuffer, RECURSION_COUNT> PortalRenderPass::m_framebuffers{};
bool PortalRenderPass::m_fboIsInit = false;

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

	if (!m_fboIsInit)
	{
		for (Framebuffer& framebuffer : m_framebuffers)
			framebuffer.Init();

		m_fboIsInit = true;
	}

}

PortalRenderPass::~PortalRenderPass()
{
	if (m_fboIsInit)
	{
		for (int i = 0; i < m_framebuffers.size(); ++i)
			m_framebuffers[i].~Framebuffer();
	
		m_fboIsInit = false;
	}
}


template <>
int PortalRenderPass::Subscribe(Portal* toGenerate)
{
	m_portals.insert(toGenerate);
	return 1;
}

template <>
void PortalRenderPass::Unsubscribe(Portal* toGenerate)
{
	m_portals.erase(toGenerate);
}

void PortalRenderPass::ComputePortalView(Portal* portal, Viewer* viewer, Framebuffer* framebuffer, PortalViewer& portalViewer)
{
	CCMaths::Matrix4 portalViewModel = portal->m_relativeLinkedPortalMatrix * viewer->m_viewMatrix.Inverse();

	CCMaths::Matrix4 portalView = portalViewModel.Inverse();

	portalViewer.m_viewMatrix = portalView;

	//Necessary for the PBR
	portalViewer.m_position = portalViewModel.position;

	const CCMaths::Matrix4& clipPlaneWorldMatrix = portal->m_linkedPortal->m_modelMatrix;
	const Vector3 clipPlaneForward = -clipPlaneWorldMatrix.back.Normalized();

	const int dot = CCMaths::Sign(Vector3::Dot(clipPlaneForward, clipPlaneWorldMatrix.position - portalViewModel.position));

	Vector3 viewSpacePos = (portalView * Vector4(clipPlaneWorldMatrix.position, 1.f)).xyz;

	portalView.position = Vector3::Zero; //To only rotate clipPlaneWorldMatrix
	Vector3 viewSpaceNormal = (portalView * Vector4(clipPlaneForward, 1.f)).xyz * static_cast<const float>(dot);

	float viewSpaceDist = -Vector3::Dot(viewSpacePos, viewSpaceNormal);

	Vector4 clipPlaneViewSpace = Vector4(viewSpaceNormal.x, viewSpaceNormal.y, viewSpaceNormal.z, viewSpaceDist);

	//Make clipping in front of the portal
	portalViewer.m_projectionMatrix = Matrix4::ObliqueProjection(clipPlaneViewSpace, viewer->m_projectionMatrix);

	float aspect = (float)framebuffer->width / (float)framebuffer->height;
	portalViewer.m_frustumPlanes = FrustumPlanes::CreateFromInverse(portalViewModel, portal->m_fovY, aspect, portal->m_near, portal->m_far);
}

PortalRenderPass::PortalViewer PortalRenderPass::SavePortalViewerParams(const Portal* portal)
{
	return PortalViewer{
			.m_viewMatrix = portal->m_viewMatrix,
			.m_projectionMatrix = portal->m_projectionMatrix,
			.m_frustumPlanes = portal->m_frustumPlanes,
			.m_position = portal->m_position,
			};
}

void PortalRenderPass::ApplyPortalViewerParams(Portal* portal, const PortalViewer& portalViewer)
{
	portal->m_viewMatrix = portalViewer.m_viewMatrix;
	portal->m_position = portalViewer.m_position;
	portal->m_frustumPlanes = portalViewer.m_frustumPlanes;
	portal->m_projectionMatrix = portalViewer.m_projectionMatrix;
}

bool PortalRenderPass::ComputePortalFBO(Portal* portal, Viewer* viewer, Framebuffer* framebuffer, int currentRecursion)
{

	if (!viewer->m_frustumPlanes.IsOnFrustum(portal->m_modelMatrix, m_quadMesh->m_aabb))
		return 0;

	m_framebuffers[(currentRecursion - 1)].UpdateFramebuffer(static_cast<float>(framebuffer->width),
		static_cast<float>(framebuffer->height));

	glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffers[(currentRecursion - 1)].FBO);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	PortalViewer portalViewer;
	ComputePortalView(portal, viewer, framebuffer, portalViewer);

	//Save previous portal viewer params
	PortalViewer previousPortalViewer = SavePortalViewerParams(portal->m_linkedPortal);

	ApplyPortalViewerParams(portal->m_linkedPortal, portalViewer);

	//int recursion = i == m_portalRecursionCount - 1 ? currentRecursion - 1 : 0;
	int recursion = currentRecursion - 1;
	portal->m_linkedPortal->DrawCustom(recursion, &m_framebuffers[currentRecursion - 1], false);

	if (!recursion)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffers[currentRecursion - 1].FBO);
		glClear(GL_DEPTH_BUFFER_BIT);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	//Reaply previous portal viewer params
	ApplyPortalViewerParams(portal->m_linkedPortal, previousPortalViewer);

	return 1;
}

void PortalRenderPass::ComputePortals(std::unordered_set<Portal*>& portals, Viewer* viewer, Framebuffer* framebuffer, int currentRecursion)
{
	for (Portal* portal : portals)
	{
		if (!portal->m_linkedPortal)
			return;

		if (ComputePortalFBO(portal, viewer, framebuffer, currentRecursion))
		{
			DrawPortal(portal, viewer, framebuffer, currentRecursion);
		}
	}
}

void PortalRenderPass::DrawPortal(Portal* portal, Viewer* viewer, Framebuffer* framebuffer, int currentRecursion)
{
	glViewport(0, 0, framebuffer->width, framebuffer->height);

	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer->FBO);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glDisable(GL_CULL_FACE);

	glUseProgram(m_program->m_shaderProgram);
	CCMaths::Matrix4 viewProjection = viewer->m_projectionMatrix * viewer->m_viewMatrix;
	glUniformMatrix4fv(glGetUniformLocation(m_program->m_shaderProgram, "uViewProjection"), 1, GL_FALSE, viewProjection.data);

	float factor = 1.f - (currentRecursion / (float)RECURSION_COUNT);
	factor *= factor;

	glUniform1f(glGetUniformLocation(m_program->m_shaderProgram, "uFogFactor"), factor);

	if (auto gpuMesh = static_cast<ElementMeshGenerator::GPUMeshBasic*>(m_quadMesh->m_gpuMesh.get()))
	{
		glBindVertexArray(gpuMesh->VAO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gpuMesh->EBO);

		glBindTextureUnit(0, m_framebuffers[currentRecursion - 1].colorTex.texID);

		glUniformMatrix4fv(glGetUniformLocation(m_program->m_shaderProgram, "uModel"), 1, GL_FALSE, portal->m_modelMatrix.data);

		glDrawElements(GL_TRIANGLES, gpuMesh->indicesCount, GL_UNSIGNED_INT, nullptr);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		glBindVertexArray(0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		glUseProgram(0);
	}
}

void PortalRenderPass::Execute(Viewer* viewer, Framebuffer* framebuffer)
{
	if (!viewer)
		return;

	if (viewer->m_currentIteration > 0)
	{
		//Todo: Add fbo in params
		ComputePortals(m_portals, viewer, framebuffer, viewer->m_currentIteration);
	}
	else
	{
		glViewport(0, 0, framebuffer->width, framebuffer->height);

		glBindFramebuffer(GL_FRAMEBUFFER, framebuffer->FBO);

		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);
		glDisable(GL_CULL_FACE);

		glUseProgram(m_program->m_shaderProgram);
		CCMaths::Matrix4 viewProjection = viewer->m_projectionMatrix * viewer->m_viewMatrix;
		glUniformMatrix4fv(glGetUniformLocation(m_program->m_shaderProgram, "uViewProjection"), 1, GL_FALSE, viewProjection.data);

		if (auto gpuMesh = static_cast<ElementMeshGenerator::GPUMeshBasic*>(m_quadMesh->m_gpuMesh.get()))
		{
			glBindVertexArray(gpuMesh->VAO);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gpuMesh->EBO);

			for (Portal* portal : m_portals)
			{
				glBindTextureUnit(0, 0u);

				glUniformMatrix4fv(glGetUniformLocation(m_program->m_shaderProgram, "uModel"), 1, GL_FALSE, portal->m_modelMatrix.data);

				glDrawElements(GL_TRIANGLES, gpuMesh->indicesCount, GL_UNSIGNED_INT, nullptr);
			}

			glBindFramebuffer(GL_FRAMEBUFFER, 0);

			glBindVertexArray(0);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

			glUseProgram(0);
		}
	}
}
