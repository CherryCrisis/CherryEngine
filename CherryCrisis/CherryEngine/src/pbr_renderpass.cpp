#include "pch.hpp"

#include "pbr_renderpass.hpp"

#include "resource_manager.hpp"

#include "framebuffer.hpp"

#include "camera.hpp"
#include "model_renderer.hpp"
#include "transform.hpp"
#include "sky_renderer.hpp"
#include "irradiance_map_renderpass.hpp"
#include "prefilter_map_renderpass.hpp"
#include "brdf_renderpass.hpp"
#include "material.hpp"
#include "texture.hpp"

#include "shadow_renderpass.hpp"
#include "viewer.hpp"

PBRRenderPass::PBRRenderPass(const char* name)
	: ARenderingRenderPass(name, "Assets/Shaders/PBR/pbrShader.vert", "Assets/Shaders/PBR/pbrShader.frag")
{
	if (m_program)
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

		glUseProgram(0);

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
}

template <>
int PBRRenderPass::Subscribe(Light* toGenerate)
{
	if (!toGenerate || !m_program)
		return -1;

	m_lights.insert(toGenerate);

	return 1;
}

template <>
int PBRRenderPass::Subscribe(ModelRenderer* toGenerate)
{
	// Generate GPU mesh
	{
		if (!m_meshGenerator.Generate(toGenerate->m_mesh.get()))
			return -1;

		m_modelRenderers.insert(toGenerate);
	}

	// Generate GPU textures
	{
		Generate(toGenerate->m_material.get());
	}

	return 1;
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

template <>
void PBRRenderPass::Unsubscribe(ModelRenderer* toGenerate)
{
	m_modelRenderers.erase(toGenerate);
}

template <>
void PBRRenderPass::Unsubscribe(Light* toGenerate)
{
	m_lights.erase(toGenerate);
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

void PBRRenderPass::Execute(Framebuffer& framebuffer, Viewer*& viewer)
{
	if (!viewer)
		return;

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glDepthFunc(GL_LESS);

	glCullFace(GL_BACK);

	glUseProgram(m_program->m_shaderProgram);
	//glClearColor(0.f, 0.f, 0.f, 1.f);
	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUniformMatrix4fv(glGetUniformLocation(m_program->m_shaderProgram, "uProjection"), 1, GL_FALSE, viewer->m_projectionMatrix.data);
	glUniformMatrix4fv(glGetUniformLocation(m_program->m_shaderProgram, "uView"), 1, GL_FALSE, viewer->m_viewMatrix.data);
	glUniform3fv(glGetUniformLocation(m_program->m_shaderProgram, "uViewPosition"), 1, viewer->m_position.data);

	const char* lightFormat = "uLights[{}]";
	std::unordered_set<Light*>::iterator lightIt = m_lights.begin();
	for (size_t lightID = 0u; lightID < 8; lightID++)
	{
		std::string iLightFormat = std::format(lightFormat, lightID) + ".{}";

		GLuint enableLoc = glGetUniformLocation(m_program->m_shaderProgram, std::format(iLightFormat, "lightType").c_str());
		glUniform1i(enableLoc, 0);

		if (lightIt == m_lights.end())
			break;

		Light* light = *lightIt;

		if (!light->m_enabled)
			glUniform1i(enableLoc, 0);
		else if (light->m_isPoint)
			glUniform1i(enableLoc, 2);
		else
			glUniform1i(enableLoc, 1);

		// TODO: Use string view

		Vector4 lightPos = Vector4(light->m_position, 1);
		GLuint posLoc = glGetUniformLocation(m_program->m_shaderProgram, std::format(iLightFormat, "position").c_str());
		glUniform4fv(posLoc, 1, lightPos.data);

		GLuint dirLoc = glGetUniformLocation(m_program->m_shaderProgram, std::format(iLightFormat, "direction").c_str());
		glUniform3fv(dirLoc, 1, light->m_position.data);

		GLuint diffLoc = glGetUniformLocation(m_program->m_shaderProgram, std::format(iLightFormat, "diffuse").c_str());
		glUniform3fv(diffLoc, 1, light->m_diffuse.data);

		Vector3 test = { 0.0f, 0.0f, 1.0f };
		GLuint IntensityLoc = glGetUniformLocation(m_program->m_shaderProgram, std::format(iLightFormat, "params").c_str()); //Intensity
		glUniform3fv(IntensityLoc, 1, test.data);

		lightID++;

		lightIt = std::next(m_lights.begin(), lightID);
	}

	for (ModelRenderer* modelRdr : m_modelRenderers)
	{

		if (!modelRdr->m_isVisible)
			continue;

		CCMaths::Matrix4 modelMat = modelRdr->m_transform->GetWorldMatrix();
		glUniformMatrix4fv(glGetUniformLocation(m_program->m_shaderProgram, "uModel"), 1, GL_FALSE, modelMat.data);

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

		Mesh* mesh = modelRdr->m_mesh.get();

		if (!mesh)
			continue;

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
