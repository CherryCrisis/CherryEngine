#include "pch.hpp"

#include "guizmo_renderpass.hpp"

#include "framebuffer.hpp"

#include "camera.hpp"
#include "model_renderer.hpp"
#include "transform.hpp"
#include "model.hpp"

#include "shadow_renderpass.hpp"
#include "portal.hpp"
#include "viewer.hpp"

#include "camera_component.hpp"
#include "audio_emitter.hpp"
#include "light_component.hpp"

#include "texture_generator.hpp"

GuizmoRenderPass::GuizmoRenderPass(const char* name)
	: ARenderingRenderPass(name, "Assets/Shaders/billboard.vert", "Assets/Shaders/billboard.frag")
{
	if (m_program)
		m_callExecute = CCCallback::BindCallback(&GuizmoRenderPass::Execute, this);

	m_quadMesh = ResourceManager::GetInstance()->AddResourceRef<Mesh>("CC_NormalizedQuad", true);

	m_audioIcon  = ResourceManager::GetInstance()->AddResource<Texture>("Internal/Icons/sound_guizmo_icon.png", true,  false, ETextureFormat::RGBA);
	m_cameraIcon = ResourceManager::GetInstance()->AddResource<Texture>("Internal/Icons/camera_guizmo_icon.png", true, false, ETextureFormat::RGBA);
	m_lightIcon  = ResourceManager::GetInstance()->AddResource<Texture>("Internal/Icons/light_guizmo_icon.png", true,  false, ETextureFormat::RGBA);

	TextureGenerator texGenerator;

	texGenerator.Generate(m_audioIcon.get());
	texGenerator.Generate(m_cameraIcon.get());
	texGenerator.Generate(m_lightIcon.get());
	
	if (!m_quadMesh->m_gpuMesh)
	{
		Mesh::CreateQuad(m_quadMesh, 1.f, 1.f);
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


void GuizmoRenderPass::Execute(Framebuffer& framebuffer, Viewer*& viewer)
{
	if (!viewer)
		return;

	glViewport(0, 0, framebuffer.colorTex.width, framebuffer.colorTex.height);

	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer.FBO);

	glEnable(GL_DEPTH_TEST);
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

	// TODO: Clean this mess



	auto gpuTexture = static_cast<TextureGenerator::GPUTextureBasic*>(m_cameraIcon->m_gpuTexture2D.get());
	glBindTextureUnit(0, gpuTexture->ID);

	for (CameraComponent* cameraComp : m_cameraComponents) 
	{
		if (Transform* t = cameraComp->GetHostPtr()->GetBehaviour<Transform>())
		{
			CCMaths::Matrix4 model = cameraComp->GetHostPtr()->GetBehaviour<Transform>()->GetWorldMatrix().NormalizedScale() * CCMaths::Matrix4::Scale(.1f);
			glUniformMatrix4fv(glGetUniformLocation(m_program->m_shaderProgram, "uModel"), 1, GL_FALSE, model.data);
			glDrawElements(GL_TRIANGLES, gpuMesh->indicesCount, GL_UNSIGNED_INT, nullptr);
		}
	}

	gpuTexture = static_cast<TextureGenerator::GPUTextureBasic*>(m_lightIcon->m_gpuTexture2D.get());
	glBindTextureUnit(0, gpuTexture->ID);

	for (LightComponent* lightComp : m_lightComponents)
	{
		if (Transform* t = lightComp->GetHostPtr()->GetBehaviour<Transform>())
		{
			CCMaths::Matrix4 model = lightComp->GetHostPtr()->GetBehaviour<Transform>()->GetWorldMatrix().NormalizedScale() * CCMaths::Matrix4::Scale(.1f);
			glUniformMatrix4fv(glGetUniformLocation(m_program->m_shaderProgram, "uModel"), 1, GL_FALSE, model.data);
			glDrawElements(GL_TRIANGLES, gpuMesh->indicesCount, GL_UNSIGNED_INT, nullptr);
		}
	}

	gpuTexture = static_cast<TextureGenerator::GPUTextureBasic*>(m_audioIcon->m_gpuTexture2D.get());
	glBindTextureUnit(0, gpuTexture->ID);

	for (AudioEmitter* audioComp : m_audioEmitters)
	{
		if (Transform* t = audioComp->GetHostPtr()->GetBehaviour<Transform>()) 
		{
			CCMaths::Matrix4 model = t->GetWorldMatrix().NormalizedScale() * CCMaths::Matrix4::Scale(.1f);
			glUniformMatrix4fv(glGetUniformLocation(m_program->m_shaderProgram, "uModel"), 1, GL_FALSE, model.data);
			glDrawElements(GL_TRIANGLES, gpuMesh->indicesCount, GL_UNSIGNED_INT, nullptr);
		}
	}
	
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	glUseProgram(0);
}
