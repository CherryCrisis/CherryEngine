#include <pch.hpp>

#include "guizmo_renderpass.hpp"

#include "audio_emitter.hpp"
#include "camera.hpp"
#include "camera_component.hpp"
#include "framebuffer.hpp"
#include "light_component.hpp"
#include "portal_component.hpp"
#include "model.hpp"
#include "model_renderer.hpp"
#include "portal.hpp"
#include "shadow_renderpass.hpp"
#include "texture_generator.hpp"
#include "transform.hpp"
#include "viewer.hpp"
#include "entity.hpp"

GuizmoRenderPass::GuizmoRenderPass(const char* name)
	: ARenderingRenderPass(name, "Assets/Shaders/billboard.vert", "Assets/Shaders/billboard.frag")
{
	if (m_program)
		m_callExecute = CCCallback::BindCallback(&GuizmoRenderPass::Execute, this);

	m_quadMesh = ResourceManager::GetInstance()->AddResourceRef<Mesh>("CC_GuizmoQuad", true);

	m_audioIcon   = ResourceManager::GetInstance()->AddResource<Texture>("Internal/Icons/sound_guizmo_icon.png", true,  false, ETextureFormat::RGBA, ETextureFilter::NEAREST_MIPMAP_NEAREST, ETextureFilter::NEAREST);
	m_cameraIcon  = ResourceManager::GetInstance()->AddResource<Texture>("Internal/Icons/camera_guizmo_icon.png", true, false, ETextureFormat::RGBA, ETextureFilter::NEAREST_MIPMAP_NEAREST, ETextureFilter::NEAREST);
	m_lightIcon   = ResourceManager::GetInstance()->AddResource<Texture>("Internal/Icons/light_guizmo_icon.png", true, false, ETextureFormat::RGBA, ETextureFilter::NEAREST_MIPMAP_NEAREST, ETextureFilter::NEAREST);
	m_portalIcon  = ResourceManager::GetInstance()->AddResource<Texture>("Internal/Icons/portal_guizmo_icon.png", true,  false, ETextureFormat::RGBA, ETextureFilter::NEAREST_MIPMAP_NEAREST, ETextureFilter::NEAREST);

	TextureGenerator texGenerator;

	texGenerator.Generate(m_audioIcon.get());
	texGenerator.Generate(m_cameraIcon.get());
	texGenerator.Generate(m_lightIcon.get());
	texGenerator.Generate(m_portalIcon.get());
	
	if (!m_quadMesh->m_gpuMesh)
	{
		Mesh::CreateQuad(m_quadMesh, .1f, .1f);
		m_meshGenerator.Generate(m_quadMesh.get());
	}
}

template <>
int GuizmoRenderPass::Subscribe(LightComponent* toGenerate)
{
	m_lightComponents.insert(toGenerate);
	return 1;
}
template <>
void GuizmoRenderPass::Unsubscribe(LightComponent* toGenerate)
{
	m_lightComponents.erase(toGenerate);
}

template <>
int GuizmoRenderPass::Subscribe(PortalComponent* toGenerate)
{
	m_portalComponents.insert(toGenerate);
	return 1;
}
template <>
void GuizmoRenderPass::Unsubscribe(PortalComponent* toGenerate)
{
	m_portalComponents.erase(toGenerate);
}

template <>
int GuizmoRenderPass::Subscribe(CameraComponent* toGenerate)
{
	m_cameraComponents.insert(toGenerate);
	return 1;
}
template <>
void GuizmoRenderPass::Unsubscribe(CameraComponent* toGenerate)
{
	m_cameraComponents.erase(toGenerate);
}

template <>
int GuizmoRenderPass::Subscribe(AudioEmitter* toGenerate)
{
	m_audioEmitters.insert(toGenerate);
	return 1;
}
template <>
void GuizmoRenderPass::Unsubscribe(AudioEmitter* toGenerate)
{
	m_audioEmitters.erase(toGenerate);
}

void GuizmoRenderPass::Execute(Viewer* viewer, Framebuffer* framebuffer)
{
	if (!viewer)
		return;

	glViewport(0, 0, framebuffer->width, framebuffer->height);

	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer->FBO);

	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glUseProgram(m_program->m_shaderProgram);
	
	CCMaths::Matrix4 view		= viewer->m_viewMatrix;
	CCMaths::Matrix4 projection = viewer->m_projectionMatrix;

	glUniformMatrix4fv(glGetUniformLocation(m_program->m_shaderProgram, "uView"), 1, GL_FALSE, view.data);
	glUniformMatrix4fv(glGetUniformLocation(m_program->m_shaderProgram, "uProjection"), 1, GL_FALSE, projection.data);

	auto gpuMesh = static_cast<ElementMeshGenerator::GPUMeshBasic*>(m_quadMesh->m_gpuMesh.get());

	if (!gpuMesh)
		return;

	glBindVertexArray(gpuMesh->VAO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gpuMesh->EBO);

	auto gpuTexture = static_cast<TextureGenerator::GPUTextureBasic*>(m_cameraIcon->m_gpuTexture2D.get());
	glBindTextureUnit(0, gpuTexture->ID);

	for (CameraComponent* cameraComp : m_cameraComponents) 
	{
		if (viewer->m_ownerCell != cameraComp->GetHost().m_cell) continue;

		if (Transform* t = cameraComp->GetHostPtr()->GetBehaviour<Transform>())
		{
			CCMaths::Matrix4 model = cameraComp->GetHostPtr()->GetBehaviour<Transform>()->GetWorldMatrix().NormalizedScale();
			glUniformMatrix4fv(glGetUniformLocation(m_program->m_shaderProgram, "uModel"), 1, GL_FALSE, model.data);
			glDrawElements(GL_TRIANGLES, gpuMesh->indicesCount, GL_UNSIGNED_INT, nullptr);
		}
	}

	gpuTexture = static_cast<TextureGenerator::GPUTextureBasic*>(m_lightIcon->m_gpuTexture2D.get());
	glBindTextureUnit(0, gpuTexture->ID);
	// #1 : change components by transforms to get them only one time instead of each frame
	for (LightComponent* lightComp : m_lightComponents)
	{
		if (viewer->m_ownerCell != lightComp->GetHost().m_cell) continue;

		if (Transform* t = lightComp->GetHostPtr()->GetBehaviour<Transform>())
		{
			CCMaths::Matrix4 model = lightComp->GetHostPtr()->GetBehaviour<Transform>()->GetWorldMatrix().NormalizedScale();
			glUniformMatrix4fv(glGetUniformLocation(m_program->m_shaderProgram, "uModel"), 1, GL_FALSE, model.data);
			glDrawElements(GL_TRIANGLES, gpuMesh->indicesCount, GL_UNSIGNED_INT, nullptr);
		}
	}

	gpuTexture = static_cast<TextureGenerator::GPUTextureBasic*>(m_audioIcon->m_gpuTexture2D.get());
	glBindTextureUnit(0, gpuTexture->ID);

	for (AudioEmitter* audioComp : m_audioEmitters)
	{
		if (viewer->m_ownerCell != audioComp->GetHost().m_cell) continue;

		if (Transform* t = audioComp->GetHostPtr()->GetBehaviour<Transform>()) 
		{
			CCMaths::Matrix4 model = t->GetWorldMatrix().NormalizedScale();
			glUniformMatrix4fv(glGetUniformLocation(m_program->m_shaderProgram, "uModel"), 1, GL_FALSE, model.data);
			glDrawElements(GL_TRIANGLES, gpuMesh->indicesCount, GL_UNSIGNED_INT, nullptr);
		}
	}

	gpuTexture = static_cast<TextureGenerator::GPUTextureBasic*>(m_portalIcon->m_gpuTexture2D.get());
	glBindTextureUnit(0, gpuTexture->ID);

	for (PortalComponent* portalComp : m_portalComponents)
	{
		if (viewer->m_ownerCell != portalComp->GetHost().m_cell) continue;

		if (Transform* t = portalComp->GetHostPtr()->GetBehaviour<Transform>())
		{
			CCMaths::Matrix4 model = t->GetWorldMatrix().NormalizedScale();
			glUniformMatrix4fv(glGetUniformLocation(m_program->m_shaderProgram, "uModel"), 1, GL_FALSE, model.data);
			glDrawElements(GL_TRIANGLES, gpuMesh->indicesCount, GL_UNSIGNED_INT, nullptr);
		}
	}
	
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	
	glEnable(GL_DEPTH_TEST);
	glUseProgram(0);
}
