#include <pch.hpp>

#include "basic_renderpass.hpp"

#include "camera.hpp"
#include "framebuffer.hpp"
#include "material.hpp"
#include "mesh.hpp"
#include "model_renderer.hpp"
#include "shadow_renderpass.hpp"
#include "shape_renderer.hpp"
#include "transform.hpp"
#include "viewer.hpp"


BasicRenderPass::BasicRenderPass(const char* name)
	: ARenderingRenderPass(name, "Assets/Shaders/LIT/basicShader.vert", "Assets/Shaders/LIT/basicShader.frag")
{
	unsigned char whiteColor[4] = { 255u, 255u, 255u, 255u };
	m_defaultTextures[ETextureType::CELSHADE_PALLET] = m_defaultTextures[ETextureType::ALBEDO] = ResourceManager::GetInstance()->AddResource<Texture>("CC_WhiteTexture", true, whiteColor);

	unsigned char blueTexture[4] = { 255u, 0u, 0u, 0u };
	m_defaultTextures[ETextureType::NORMAL_MAP] = ResourceManager::GetInstance()->AddResource<Texture>("CC_BlueTexture", true, blueTexture);


	for (auto& [texType, texRef] : m_defaultTextures)
		m_textureGenerator.Generate(texRef.get());
}

void BasicRenderPass::OnPreDrawBind()
{
	glUseProgram(m_program->m_shaderProgram);

	glUniform1i(glGetUniformLocation(m_program->m_shaderProgram, "uMaterial.albedoTex"), 0);
	glUniform1i(glGetUniformLocation(m_program->m_shaderProgram, "uMaterial.normalMap"), 1);
	glUniform1i(glGetUniformLocation(m_program->m_shaderProgram, "uMaterial.celshadePallet"), 2);

	m_callExecute = CCCallback::BindCallback(&BasicRenderPass::Execute, this);

	glUniformBlockBinding(m_program->m_shaderProgram, glGetUniformBlockIndex(m_program->m_shaderProgram, "uLightsBlock"), LightGenerator::UBOBindingPoint);

	glUseProgram(0);
}

void BasicRenderPass::BindTexture(Material* material, ETextureType textureType, int id)
{
	// Get correct texture from type
	Texture* texture = material->m_textures[textureType].get();

	// If is does not exist and its gpuTex too, get the default texture
	auto& gpuTexPtr = texture && texture->m_gpuTexture2D ? texture->m_gpuTexture2D : m_defaultTextures[textureType]->m_gpuTexture2D;

	auto gpuTexture = static_cast<TextureGenerator::GPUTextureBasic*>(gpuTexPtr.get());
	glBindTextureUnit(id, gpuTexture->ID);
}

template <>
int BasicRenderPass::Subscribe(Light* toGenerate)
{
	if (!toGenerate || !m_program)
		return -1;

	if (!m_lightGenerator.Generate(toGenerate))
		return -1;

	m_lights.push_back(toGenerate);

	return 1;
}

template <>
int BasicRenderPass::Subscribe(MeshRenderer* toGenerate)
{
	// Generate GPU mesh
	{
		if (!m_meshGenerator.Generate(toGenerate->m_mesh.get()))
			return -1;

		m_models.insert(toGenerate);
	}

	// Generate GPU textures
	{
		Generate(toGenerate->m_material.get());
	}

	return 1;
}

template <>
void BasicRenderPass::Unsubscribe(MeshRenderer* toGenerate)
{
	m_models.erase(toGenerate);
}

void BasicRenderPass::Generate(Material* toGenerate)
{
	if (!toGenerate)
		return;

	// Albedo texture
	if (Texture* albedoTexture = toGenerate->m_textures[ETextureType::ALBEDO].get())
		m_textureGenerator.Generate(albedoTexture);

	// Normal texture
	if (Texture* normalMap = toGenerate->m_textures[ETextureType::NORMAL_MAP].get())
		m_textureGenerator.Generate(normalMap);
}

template <>
void BasicRenderPass::Unsubscribe(Light* toGenerate)
{
	auto lightIt = std::find(m_lights.begin(), m_lights.end(), toGenerate);

	if (lightIt != m_lights.end())
		m_lights.erase(lightIt);
}

void BasicRenderPass::Execute(Viewer* viewer, Framebuffer* framebuffer)
{
	if (!viewer)
		return;

	glViewport(0, 0, framebuffer->width, framebuffer->height);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer->FBO);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glDepthFunc(GL_LESS);

	glCullFace(GL_BACK);

	glUseProgram(m_program->m_shaderProgram);

	CCMaths::Matrix4 view = viewer->m_viewMatrix;
	CCMaths::Matrix4 projection = viewer->m_projectionMatrix;

	glUniformMatrix4fv(glGetUniformLocation(m_program->m_shaderProgram, "uView"), 1, GL_FALSE, view.data);
	glUniformMatrix4fv(glGetUniformLocation(m_program->m_shaderProgram, "uProjection"), 1, GL_FALSE, projection.data);

	glUniform3fv(glGetUniformLocation(m_program->m_shaderProgram, "uViewPosition"), 1, (-viewer->m_viewMatrix.position).data);

	for (size_t lightID = 0u; lightID < std::min<size_t>(m_lights.size(), 8u); lightID++)
	{
		Light* light = m_lights[lightID];
		if (auto gpuLight = static_cast<LightGenerator::GPULightBasic*>(light->m_gpuLight.get()))
		{
			GLuint textureNum = 4 + (GLsizei)gpuLight->index;
			glUniform1i(glGetUniformLocation(m_program->m_shaderProgram, "uShadowMaps") + (GLsizei)gpuLight->index, textureNum);
			glBindTextureUnit(textureNum, gpuLight->TexID);
		}
	}

	for (MeshRenderer* modelRdr : m_models)
	{
		if (!modelRdr->m_isVisible)
			continue;

		Mesh* mesh = modelRdr->m_mesh.get();

		if (!mesh)
			continue;

		CCMaths::Matrix4 modelMat = modelRdr->m_transform->GetWorldMatrix();

		if (!viewer->m_frustumPlanes.IsOnFrustum(modelMat, mesh->m_aabb))
			continue;

		glUniformMatrix4fv(glGetUniformLocation(m_program->m_shaderProgram, "uModel"), 1, GL_FALSE, modelMat.data);
		glUniform1i(glGetUniformLocation(m_program->m_shaderProgram, "uIsSelected"), 0);

		if (Material* material = modelRdr->m_material.get())
		{
			glUniform3fv(glGetUniformLocation(m_program->m_shaderProgram, "uMaterial.ambientCol"), 1, material->m_ambient.data);
			glUniform3fv(glGetUniformLocation(m_program->m_shaderProgram, "uMaterial.diffuseCol"), 1, material->m_diffuse.data);
			glUniform3fv(glGetUniformLocation(m_program->m_shaderProgram, "uMaterial.specularCol"), 1, material->m_specular.data);
			glUniform3fv(glGetUniformLocation(m_program->m_shaderProgram, "uMaterial.emissiveCol"), 1, material->m_emissive.data);
			glUniform1f(glGetUniformLocation(m_program->m_shaderProgram, "uMaterial.shininess"), material->m_shininess);
			glUniform1f(glGetUniformLocation(m_program->m_shaderProgram, "uMaterial.hasCelShade"), material->m_hasCelshade);

			BindTexture(material, ETextureType::ALBEDO, 0);
			BindTexture(material, ETextureType::NORMAL_MAP, 1);

			if (material->m_hasCelshade)
				BindTexture(material, ETextureType::CELSHADE_PALLET, 2);
		}

		auto gpuMesh = static_cast<ElementTBNGenerator::GPUMeshBasic*>(mesh->m_gpuMesh.get());

		if (!gpuMesh)
			continue;

		glBindVertexArray(gpuMesh->VAO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gpuMesh->EBO);
		glDrawElements(GL_TRIANGLES, gpuMesh->indicesCount, GL_UNSIGNED_INT, nullptr);
	}

	glUseProgram(0);

	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}
