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

GuizmoRenderpass::GuizmoRenderpass(const char* name)
	: ARenderingRenderPass(name, "Assets/Shaders/portalShader.vert", "Assets/Shaders/portalShader.frag")
{
	if (m_program)
		m_callExecute = CCCallback::BindCallback(&GuizmoRenderpass::Execute, this);

	m_quadMesh = ResourceManager::GetInstance()->AddResourceRef<Mesh>("CC_NormalizedQuad", true);

	m_audioIcon  = ResourceManager::GetInstance()->AddResource<Texture>("Internal/Icons/sound_guizmo_icon.png", true);
	m_cameraIcon = ResourceManager::GetInstance()->AddResource<Texture>("Internal/Icons/camera_guizmo_icon.png", true);
	m_lightIcon  = ResourceManager::GetInstance()->AddResource<Texture>("Internal/Icons/light_guizmo_icon.png", true);

	if (!m_quadMesh->m_gpuMesh)
	{
		Mesh::CreateQuad(m_quadMesh, 1.f, 1.f);
		m_meshGenerator.Generate(m_quadMesh.get());
	}
}

template <class Tsearched>
Tsearched* GetDerived() {
	for (auto c : m_classes) {
		if (Tsearched* ptr = dynamic_cast<Tsearched*>(c))
			return ptr;
	}
	return nullptr;
}

template <>
int GuizmoRenderpass::Subscribe(Behaviour* toGenerate)
{
	// check if behaviour is type of light, camera or audio emitter
	if (CameraComponent* comp = dynamic_cast<CameraComponent*>(toGenerate))
		m_cameraComponents.insert(comp);

	if (AudioEmitter* comp = dynamic_cast<AudioEmitter*>(toGenerate))
		m_audioEmitters.insert(comp); 

	if (LightComponent* comp = dynamic_cast<LightComponent*>(toGenerate))
		m_lightComponents.insert(comp);
	
	return 1;
}

template <>
void GuizmoRenderpass::Unsubscribe(Behaviour* toGenerate)
{
	if (CameraComponent* comp = dynamic_cast<CameraComponent*>(toGenerate))
		m_cameraComponents.erase(comp);

	if (AudioEmitter* comp = dynamic_cast<AudioEmitter*>(toGenerate))
		m_audioEmitters.erase(comp);

	if (LightComponent* comp = dynamic_cast<LightComponent*>(toGenerate))
		m_lightComponents.erase(comp);
}

void GuizmoRenderpass::Execute(Framebuffer& framebuffer, Viewer*& viewer)
{
	if (!viewer)
		return;

	glViewport(0, 0, framebuffer.colorTex.width, framebuffer.colorTex.height);

	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer.FBO);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glDepthFunc(GL_LESS);

	glCullFace(GL_BACK);

	glClearColor(0.f, 0.f, 0.f, 1.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

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

	for (CameraComponent* cameraComp : m_cameraComponents) 
	{
		CCMaths::Matrix4 model = cameraComp->GetHostPtr()->GetBehaviour<Transform>()->GetWorldMatrix();
		glUniformMatrix4fv(glGetUniformLocation(m_program->m_shaderProgram, "uModel"), 1, GL_FALSE, model.data);

		auto gpuTexture = static_cast<TextureGenerator::GPUTextureBasic*>(m_cameraIcon->m_gpuTexture2D.get());
		glBindTextureUnit(0, gpuTexture->ID);

		glDrawElements(GL_TRIANGLES, gpuMesh->indicesCount, GL_UNSIGNED_INT, nullptr);
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	glUseProgram(0);
}
