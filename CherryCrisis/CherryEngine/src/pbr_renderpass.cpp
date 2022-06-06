#include <pch.hpp>

#include "pbr_renderpass.hpp"

#include "resource_manager.hpp"

#include "brdf_renderpass.hpp"
#include "camera.hpp"
#include "framebuffer.hpp"
#include "irradiance_map_renderpass.hpp"
#include "material.hpp"
#include "model_renderer.hpp"
#include "prefilter_map_renderpass.hpp"
#include "shadow_renderpass.hpp"
#include "shape_renderer.hpp"
#include "sky_renderer.hpp"
#include "texture.hpp"
#include "transform.hpp"
#include "viewer.hpp"
#include "portal_teleporter_component.hpp"


PBRRenderPass::PBRRenderPass(const char* name)
	: ARenderingRenderPass(name, "Assets/Shaders/PBR/pbrShader.vert", "Assets/Shaders/PBR/pbrShader.frag")
{
	if (m_program)
		OnPreDrawBind();

	unsigned char whiteColor[4] = { 255u, 255u, 255u, 255u };
	std::shared_ptr<Texture> whiteTexture = ResourceManager::GetInstance()->AddResource<Texture>("CC_WhiteTexture", true, whiteColor);
	m_defaultTextures[ETextureType::ALBEDO] = whiteTexture;
	m_defaultTextures[ETextureType::SPECULAR] = whiteTexture;
	m_defaultTextures[ETextureType::METALLIC] = whiteTexture;
	m_defaultTextures[ETextureType::ROUGHNESS] = whiteTexture;
	m_defaultTextures[ETextureType::AO] = whiteTexture;

	unsigned char blueTexture[4] = { 255u, 0u, 0u, 0u };
	m_defaultTextures[ETextureType::NORMAL_MAP] = ResourceManager::GetInstance()->AddResource<Texture>("CC_BlueTexture", true, blueTexture);

	for (auto& [texType, texRef] : m_defaultTextures)
		m_textureGenerator.Generate(texRef.get());
}

void PBRRenderPass::OnPreDrawBind()
{
	glUseProgram(m_program->m_shaderProgram);
	glUniform1i(glGetUniformLocation(m_program->m_shaderProgram, "uMaterial.albedoMap"), 0);
	glUniform1i(glGetUniformLocation(m_program->m_shaderProgram, "uMaterial.normalMap"), 1);
	glUniform1i(glGetUniformLocation(m_program->m_shaderProgram, "uMaterial.specularMap"), 2);
	glUniform1i(glGetUniformLocation(m_program->m_shaderProgram, "uMaterial.metallicMap"), 3);
	glUniform1i(glGetUniformLocation(m_program->m_shaderProgram, "uMaterial.roughnessMap"), 4);
	glUniform1i(glGetUniformLocation(m_program->m_shaderProgram, "uMaterial.aoMap"), 5);
	glUniform1i(glGetUniformLocation(m_program->m_shaderProgram, "irradianceMap"), 6);
	glUniform1i(glGetUniformLocation(m_program->m_shaderProgram, "prefilterMap"), 7);
	glUniform1i(glGetUniformLocation(m_program->m_shaderProgram, "brdfLUT"), 8);

	m_callExecute = CCCallback::BindCallback(&PBRRenderPass::Execute, this);

	GLuint lightBlock = glGetUniformBlockIndex(m_program->m_shaderProgram, "uLightsBlock");
	glUniformBlockBinding(m_program->m_shaderProgram, lightBlock, LightGenerator::UBOBindingPoint);

	glUseProgram(0);
}

template <>
int PBRRenderPass::Subscribe(Light* toGenerate)
{
	if (!toGenerate || !m_program)
		return -1;

	if (!m_lightGenerator.Generate(toGenerate))
		return -1;

	m_lights.push_back(toGenerate);

	return 1;
}

template <>
void PBRRenderPass::Unsubscribe(Light* toGenerate)
{
	auto lightIt = std::find(m_lights.begin(), m_lights.end(), toGenerate);

	if (lightIt != m_lights.end())
		m_lights.erase(lightIt);
}

template <>
int PBRRenderPass::Subscribe(MeshRenderer* toGenerate)
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
void PBRRenderPass::Unsubscribe(MeshRenderer* toGenerate)
{
	m_models.erase(toGenerate);
}

template <>
int PBRRenderPass::Subscribe(SkyRenderer* toGenerate)
{
	if (!toGenerate)
		return -1;

	m_skyRenderer = toGenerate;

	return 1;
}

template <>
void PBRRenderPass::Unsubscribe(SkyRenderer* toGenerate)
{
	if (m_skyRenderer == toGenerate)
	{
		m_skyRenderer = nullptr;
	}
}

void PBRRenderPass::Generate(Material* toGenerate)
{
	if (!toGenerate)
		return;

	// Albedo texture
	if (Texture* albedoTexture = toGenerate->m_textures[ETextureType::ALBEDO].get())
		m_textureGenerator.Generate(albedoTexture);

	// Normal texture
	if (Texture* normalMap = toGenerate->m_textures[ETextureType::NORMAL_MAP].get())
		m_textureGenerator.Generate(normalMap);

	if (Texture* specularMap = toGenerate->m_textures[ETextureType::SPECULAR].get())
		m_textureGenerator.Generate(specularMap);

	if (Texture* metallicMap = toGenerate->m_textures[ETextureType::METALLIC].get())
		m_textureGenerator.Generate(metallicMap);

	if (Texture* roughnessMap = toGenerate->m_textures[ETextureType::ROUGHNESS].get())
		m_textureGenerator.Generate(roughnessMap);

	if (Texture* aoMap = toGenerate->m_textures[ETextureType::AO].get())
		m_textureGenerator.Generate(aoMap);
}

void PBRRenderPass::BindTexture(Material* material, ETextureType textureType, int id)
{
	// Get correct texture from type
	Texture* texture = material->m_textures[textureType].get();

	// TODO: Add multiple default textures
	// If is does not exist and its gpuTex too, get the default texture
	auto& gpuTexPtr = texture && texture->m_gpuTexture2D ? texture->m_gpuTexture2D : m_defaultTextures[textureType]->m_gpuTexture2D;

	auto gpuTexture = static_cast<TextureGenerator::GPUTextureBasic*>(gpuTexPtr.get());
	glBindTextureUnit(id, gpuTexture->ID);
}

void PBRRenderPass::Execute(Viewer*& viewer)
{
	if (!viewer)
		return;

	const Framebuffer& framebuffer = *viewer->m_framebuffer;

	glViewport(0, 0, framebuffer.width, framebuffer.height);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer.FBO);


	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glDepthFunc(GL_LESS);

	glCullFace(GL_BACK);

	glUseProgram(m_program->m_shaderProgram);

	glUniformMatrix4fv(glGetUniformLocation(m_program->m_shaderProgram, "uProjection"), 1, GL_FALSE, viewer->m_projectionMatrix.data);
	glUniformMatrix4fv(glGetUniformLocation(m_program->m_shaderProgram, "uView"), 1, GL_FALSE, viewer->m_viewMatrix.data);
	glUniform3fv(glGetUniformLocation(m_program->m_shaderProgram, "uViewPosition"), 1, viewer->m_position.data);

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

		glUniform1f(glGetUniformLocation(m_program->m_shaderProgram, "uIsSlice"), modelRdr->m_isSlice);
		glUniform3fv(glGetUniformLocation(m_program->m_shaderProgram, "uSliceCentre"), 1, modelRdr->m_sliceCentre.data);
		glUniform3fv(glGetUniformLocation(m_program->m_shaderProgram, "uSliceNormal"), 1, modelRdr->m_sliceNormal.data);

		glUniform1i(glGetUniformLocation(m_program->m_shaderProgram, "uIsSelected"), 0);

		if (Material* material = modelRdr->m_material.get())
		{

			glUniform1i(glGetUniformLocation(m_program->m_shaderProgram, "hasIrradianceMap"), 1);
			glUniform1i(glGetUniformLocation(m_program->m_shaderProgram, "uMaterial.hasNormalMap"), material->m_hasNormal);
			glUniform3fv(glGetUniformLocation(m_program->m_shaderProgram, "uMaterial.albedo"), 1, material->m_diffuse.data);
			glUniform1f(glGetUniformLocation(m_program->m_shaderProgram, "uMaterial.specular"), material->m_specularFactor);
			glUniform1f(glGetUniformLocation(m_program->m_shaderProgram, "uMaterial.metallic"), material->m_metallicFactor);
			glUniform1f(glGetUniformLocation(m_program->m_shaderProgram, "uMaterial.roughness"), material->m_roughnessFactor);
			glUniform1f(glGetUniformLocation(m_program->m_shaderProgram, "uMaterial.ao"), material->m_ao);
			glUniform1f(glGetUniformLocation(m_program->m_shaderProgram, "uMaterial.clearCoat"), material->m_clearCoatFactor);
			glUniform1f(glGetUniformLocation(m_program->m_shaderProgram, "uMaterial.clearCoatRoughness"), material->m_clearCoatRoughnessFactor);

			BindTexture(material, ETextureType::ALBEDO, 0);
			BindTexture(material, ETextureType::NORMAL_MAP, 1);
			BindTexture(material, ETextureType::SPECULAR, 2);
			BindTexture(material, ETextureType::METALLIC, 3);
			BindTexture(material, ETextureType::ROUGHNESS, 4);
			BindTexture(material, ETextureType::AO, 5);

			if (m_skyRenderer)
			{
				if (Texture* spheremap = m_skyRenderer->m_texture.get())
				{
					if (auto gpuIrradianceMap = static_cast<IrradianceMapRenderPass::GPUIrradianceMapSphereMap*>(spheremap->m_gpuIrradiancemap.get()))
					{
						glBindTextureUnit(6, gpuIrradianceMap->ID);
					}

					if (auto gpuPrefilterMap = static_cast<PrefilterMapRenderPass::GPUPrefilterMapSphereMap*>(spheremap->m_gpuPrefilterMap.get()))
					{
						glBindTextureUnit(7, gpuPrefilterMap->ID);
					}

					if (auto gpuBRDF = static_cast<BRDFRenderPass::GPUBRDFSphereMap*>(m_skyRenderer->m_gpuBRDF.get()))
					{
						glBindTextureUnit(8, gpuBRDF->ID);
					}
				}
			}
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
