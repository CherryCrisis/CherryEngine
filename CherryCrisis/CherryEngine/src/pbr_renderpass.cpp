#include "pch.hpp"

#include "pbr_renderpass.hpp"

#include "resource_manager.hpp"

#include "framebuffer.hpp"

#include "camera.hpp"
#include "model_renderer.hpp"
#include "transform.hpp"
#include "model.hpp"
#include "skydome.hpp"
#include "irradiance_map_renderpass.hpp"
#include "prefilter_map_renderpass.hpp"
#include "brdf_renderpass.hpp"

#include "shadow_renderpass.hpp"
#include "viewer.hpp"

PBRRenderPass::PBRRenderPass(const char* name)
	: ARenderingRenderPass(name, "Assets/pbrShaders/pbrShader.vert", "Assets/pbrShaders/pbrShader.frag")
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

		m_defaultTexture = ResourceManager::GetInstance()->AddResource<Texture>("Internal/PBR/defaultTexture.png", true);
		Subscribe(m_defaultTexture.get());
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
	Model* model = toGenerate->m_model.get();

	if (!model)
		return -1;

	// Generate GPU mesh
	{
		if (!m_meshGenerator.Generate(model->m_mesh.get()))
			return -1;

		m_modelRenderers.insert(toGenerate);
	}

	// Generate GPU textures
	{
		if (Material* material = model->m_material.get())
			Subscribe(material);
	}

	return 1;
}

template <>
int PBRRenderPass::Subscribe(Material* toGenerate)
{
	if (!toGenerate)
		return -1;

	// Albedo texture
	if (Texture* albedoTexture = toGenerate->m_textures[ETextureType::ALBEDO].get())
		Subscribe(albedoTexture);

	// Normal texture
	if (Texture* normalMap = toGenerate->m_textures[ETextureType::NORMAL_MAP].get())
		Subscribe(normalMap);

	if (Texture* specularMap = toGenerate->m_textures[ETextureType::SPECULAR].get())
		Subscribe(specularMap);

	if (Texture* metallicMap = toGenerate->m_textures[ETextureType::METALLIC].get())
		Subscribe(metallicMap);

	if (Texture* roughnessMap = toGenerate->m_textures[ETextureType::ROUGHNESS].get())
		Subscribe(roughnessMap);

	if (Texture* aoMap = toGenerate->m_textures[ETextureType::AO].get())
		Subscribe(aoMap);


	return 1;
}

template <>
int PBRRenderPass::Subscribe(Texture* toGenerate)
{
	if (!toGenerate)
		return -1;

	if (toGenerate->m_gpuTexture)
		return 0;

	if (!toGenerate->GetData() || toGenerate->GetWidth() <= 0 || toGenerate->GetHeight() <= 0)
		return -1;

	toGenerate->m_gpuTexture = std::make_unique<TextureGenerator::GPUTextureBasic>(toGenerate);

	return 1;
}

template <>
int PBRRenderPass::Subscribe(Skydome* toGenerate)
{
	if (!toGenerate)
		return -1;

	m_skydome = toGenerate;

	return 1;
}

template <>
void PBRRenderPass::Unsubscribe(Skydome* toGenerate)
{
	if (m_skydome == toGenerate)
	{
		m_skydome = nullptr;
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
	if (Texture* texture = material->m_textures[textureType].get())
	{
		if (auto gpuTexture = static_cast<TextureGenerator::GPUTextureBasic*>(texture->m_gpuTexture.get()))
		{
			glBindTextureUnit(id, gpuTexture->ID);
		}
		else
		{
			if (auto gpuDefaultTexture = static_cast<TextureGenerator::GPUTextureBasic*>(m_defaultTexture->m_gpuTexture.get()))
			{
				glBindTextureUnit(id, gpuDefaultTexture->ID);
			}
		}
	}
	else
	{
		if (auto gpuDefaultTexture = static_cast<TextureGenerator::GPUTextureBasic*>(m_defaultTexture->m_gpuTexture.get()))
		{
			glBindTextureUnit(id, gpuDefaultTexture->ID);
		}
	}
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
	glUniform3fv(glGetUniformLocation(m_program->m_shaderProgram, "uViewPosition"), 1, viewer->position.data);

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

		Model* model = modelRdr->m_model.get();

		if (!model)
			continue;

		CCMaths::Matrix4 modelMat = modelRdr->m_transform->GetWorldMatrix();
		glUniformMatrix4fv(glGetUniformLocation(m_program->m_shaderProgram, "uModel"), 1, GL_FALSE, modelMat.data);

		if (Material* material = model->m_material.get())
		{
			glUniform1i(glGetUniformLocation(m_program->m_shaderProgram, "hasIrradianceMap"), 1);
			glUniform1i(glGetUniformLocation(m_program->m_shaderProgram, "uMaterial.hasNormalMap"), 0);
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

			if (Spheremap* spheremap = m_skydome->m_spheremap.get())
			{
				if (auto gpuIrradianceMap = static_cast<IrradianceMapRenderPass::GPUIrradianceMapSphereMap*>(spheremap->m_gpuIrradiancemap.get()))
				{
					glBindTextureUnit(6, gpuIrradianceMap->ID);
				}

				if (auto gpuPrefilterMap = static_cast<PrefilterMapRenderPass::GPUPrefilterMapSphereMap*>(spheremap->m_gpuPrefilterMap.get()))
				{
					glBindTextureUnit(7, gpuPrefilterMap->ID);
				}

				if (auto gpuBRDF = static_cast<BRDFRenderPass::GPUBRDFSphereMap*>(spheremap->m_gpuBrdf.get()))
				{
					glBindTextureUnit(8, gpuBRDF->ID);
				}
			}
		}

		Mesh* mesh = model->m_mesh.get();

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
