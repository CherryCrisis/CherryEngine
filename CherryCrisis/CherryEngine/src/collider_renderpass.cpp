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

	m_box		= ResourceManager::GetInstance()->AddResource<Mesh>("CC_NormalizedCube", true, EMeshShape::CUBE, 0.5f, 0.5f, 0.5f);
	m_sphere	= ResourceManager::GetInstance()->AddResource<Mesh>("CC_NormalizedSphere", true, EMeshShape::SPHERE, 0.5f, 8.f, 17.f);
	m_cylinder	= ResourceManager::GetInstance()->AddResource<Mesh>("CC_NormalizedCylinder", true, EMeshShape::CYLINDER, 0.5f, 0.5f, 16.f);

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

void ColliderRenderPass::Execute(Framebuffer& framebuffer, Viewer*& viewer)
{
	if (!viewer)
		return;

	glViewport(0, 0, framebuffer.colorTex.width, framebuffer.colorTex.height);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer.FBO);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glDepthFunc(GL_ALWAYS);

	glCullFace(GL_BACK);
	glPolygonMode(GL_FRONT, GL_LINE);

	glUseProgram(m_program->m_shaderProgram);

	CCMaths::Matrix4 viewProjection = viewer->m_projectionMatrix * viewer->m_viewMatrix;
	glUniformMatrix4fv(glGetUniformLocation(m_program->m_shaderProgram, "uViewProjection"), 1, GL_FALSE, viewProjection.data);

	for (Collider* collider : m_colliders)
	{
		if (!collider->m_isVisible)
			continue;

		CapsuleCollider* capsColl = nullptr;
		switch (collider->m_type)
		{
		case EColliderType::BOX:
			DrawMesh(m_box.get(), collider->GetTranformMatrix());
			break;

		case EColliderType::SPHERE:
			DrawMesh(m_sphere.get(), collider->GetTranformMatrix());
			break;

		case EColliderType::CAPSULE:
			capsColl = reinterpret_cast<CapsuleCollider*>(collider);
			DrawMesh(m_sphere.get(), capsColl->GetTopMatrix());
			DrawMesh(m_sphere.get(), capsColl->GetBotMatrix());
			DrawMesh(m_cylinder.get(), capsColl->GetTranformMatrix());
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

	CCMaths::Vector4 color = { 0.f, 1.f, 0.f, 1.f };
	glUniform4fv(glGetUniformLocation(m_program->m_shaderProgram, "uColor"), 1, color.data);

	if (!mesh)
		return;

	auto gpuMesh = static_cast<ElementTBNGenerator::GPUMeshBasic*>(mesh->m_gpuMesh.get());

	if (!gpuMesh)
		return;

	glBindVertexArray(gpuMesh->VAO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gpuMesh->EBO);
	glDrawElements(GL_TRIANGLES, gpuMesh->indicesCount, GL_UNSIGNED_INT, nullptr);
}