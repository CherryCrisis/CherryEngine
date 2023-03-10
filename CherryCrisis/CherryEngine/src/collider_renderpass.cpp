#include <pch.hpp>

#include "collider_renderpass.hpp"

#include "box_collider.hpp"
#include "camera.hpp"
#include "capsule_collider.hpp"
#include "framebuffer.hpp"
#include "mesh.hpp"
#include "model_renderer.hpp"
#include "sphere_collider.hpp"
#include "transform.hpp"
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

	m_box		= ResourceManager::GetInstance()->AddResource<Mesh>("CC_ColliderCube", true, EMeshShape::CUBE, 1.f, 1.f, 1.f);
	m_sphere	= ResourceManager::GetInstance()->AddResource<Mesh>("CC_ColliderSphere", true, EMeshShape::SPHERE, 1.f, 9.f, 17.f);
	m_cylinder	= ResourceManager::GetInstance()->AddResource<Mesh>("CC_ColliderCylinder", true, EMeshShape::CYLINDER, 1.f, 1.f, 16.f);

	m_meshGenerator.Generate(m_box.get());
	m_meshGenerator.Generate(m_sphere.get());
	m_meshGenerator.Generate(m_cylinder.get());
}

template <>
int ColliderRenderPass::Subscribe(Collider* toGenerate)
{
	m_colliders.insert(toGenerate);

	return 1;
}

template <>
void ColliderRenderPass::Unsubscribe(Collider* toGenerate)
{
	m_colliders.erase(toGenerate);
}

void ColliderRenderPass::Execute(Viewer* viewer, Framebuffer* framebuffer)
{
	if (!viewer)
		return;

	glViewport(0, 0, framebuffer->width, framebuffer->height);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer->FBO);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	glCullFace(GL_BACK);
	glPolygonMode(GL_FRONT, GL_LINE);

	glUseProgram(m_program->m_shaderProgram);

	CCMaths::Matrix4 viewProjection = viewer->m_projectionMatrix * viewer->m_viewMatrix;
	glUniformMatrix4fv(glGetUniformLocation(m_program->m_shaderProgram, "uViewProjection"), 1, GL_FALSE, viewProjection.data);

	for (Collider* collider : m_colliders)
	{
		if (collider->m_isVisible)
		{
			glDepthFunc(GL_ALWAYS);
			m_color = { 0.f, 1.f, 0.f, 1.f };
		}
		else
		{
			glDepthFunc(GL_LESS);
			m_color = { 0.f, 0.7f, 0.f, 1.f };
		}

		CapsuleCollider* capsColl = nullptr;
		switch (collider->m_type)
		{
		case EColliderType::BOX:
			DrawMesh(m_box.get(), collider->GetModelMatrix());
			break;

		case EColliderType::SPHERE:
			DrawMesh(m_sphere.get(), collider->GetModelMatrix());
			break;

		case EColliderType::CAPSULE:
			capsColl = reinterpret_cast<CapsuleCollider*>(collider);
			DrawMesh(m_sphere.get(), capsColl->GetTopMatrix());
			DrawMesh(m_sphere.get(), capsColl->GetBotMatrix());
			DrawMesh(m_cylinder.get(), capsColl->GetModelMatrix());
			break;

		default:
			break;
		}
	}

	glUseProgram(0);

	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void ColliderRenderPass::DrawMesh(Mesh* mesh, const CCMaths::Matrix4& model)
{
	glUniformMatrix4fv(glGetUniformLocation(m_program->m_shaderProgram, "uModel"), 1, GL_FALSE, model.data);

	glUniform4fv(glGetUniformLocation(m_program->m_shaderProgram, "uColor"), 1, m_color.data);

	if (!mesh)
		return;

	auto gpuMesh = static_cast<ElementTBNGenerator::GPUMeshBasic*>(mesh->m_gpuMesh.get());

	if (!gpuMesh)
		return;

	glBindVertexArray(gpuMesh->VAO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gpuMesh->EBO);
	glDrawElements(GL_TRIANGLES, gpuMesh->indicesCount, GL_UNSIGNED_INT, nullptr);
}
