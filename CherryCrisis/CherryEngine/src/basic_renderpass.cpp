#include "pch.hpp"

#include "basic_renderpass.hpp"

#include "framebuffer.hpp"

#include "camera.hpp"
#include "model_renderer.hpp"
#include "transform.hpp"
#include "model.hpp"

#include "shadow_renderpass.hpp"
#include "viewer.hpp"


BasicRenderPass::BasicRenderPass(const char* name)
	: ARenderingRenderPass(name, "Assets/basicShader.vert", "Assets/basicShader.frag")
{
	if (m_program)
	{
		glUseProgram(m_program->m_shaderProgram);

		glUniform1i(glGetUniformLocation(m_program->m_shaderProgram, "uMaterial.albedoTex"), 0);
		glUniform1i(glGetUniformLocation(m_program->m_shaderProgram, "uMaterial.normalMap"), 1);

		m_callExecute = CCCallback::BindCallback(&BasicRenderPass::Execute, this);

		glUseProgram(0);
	}
}

template <>
int BasicRenderPass::Subscribe(Light* toGenerate)
{
	if (!toGenerate || !m_program)
		return -1;

	m_lights.insert(toGenerate);

	return 1;
}

template <>
int BasicRenderPass::Subscribe(ModelRenderer* toGenerate)
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

	toGenerate->m_onMaterialSet.Bind(&BasicRenderPass::Generate, this);

	// Generate GPU textures
	{
		if (Material* material = model->m_material.get())
			Generate(material);
	}

	return 1;
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
void BasicRenderPass::Unsubscribe(ModelRenderer* toGenerate)
{
	m_modelRenderers.erase(toGenerate);
}

template <>
void BasicRenderPass::Unsubscribe(Light* toGenerate)
{
	m_lights.erase(toGenerate);
}

void BasicRenderPass::Execute(Framebuffer& framebuffer, Viewer*& viewer)
{
	if (!viewer)
		return;

	glViewport(0, 0, framebuffer.colorTex.width, framebuffer.colorTex.height);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer.FBO);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glDepthFunc(GL_LESS);

	glCullFace(GL_BACK);

	glUseProgram(m_program->m_shaderProgram);
	glClearColor(0.f, 0.f, 0.f, 1.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	CCMaths::Matrix4 viewProjection = viewer->m_projectionMatrix * viewer->m_viewMatrix;
	glUniformMatrix4fv(glGetUniformLocation(m_program->m_shaderProgram, "uViewProjection"), 1, GL_FALSE, viewProjection.data);

	glUniform3fv(glGetUniformLocation(m_program->m_shaderProgram, "uViewPosition"), 1, (-viewer->m_viewMatrix.position).data);

	const char* lightFormat = "uLights[{}]";
	std::unordered_set<Light*>::iterator lightIt = m_lights.begin();

	// TODO: Set shader define as upper bound
	for (size_t lightID = 0u; lightID < 8; lightID++)
	{
		std::string iLightFormat = std::format(lightFormat, lightID) + ".{}";

		GLuint enableLoc = glGetUniformLocation(m_program->m_shaderProgram, std::format(iLightFormat, "isEnabled").c_str());
		glUniform1i(enableLoc, false);
		
		if (lightIt == m_lights.end())
			break;

		Light* light = *lightIt;
		glUniform1i(enableLoc, light->m_enabled);

		auto* gpuLight = static_cast<ShadowRenderPass::GPUShadowLight*>(light->m_gpuLight.get());

		if (!gpuLight)
		{
			lightID++;
			continue;
		}

		glUniform1i(glGetUniformLocation(m_program->m_shaderProgram, "uShadowMaps") + (GLsizei)lightID, 3 + (GLsizei)lightID);
		glBindTextureUnit(3 + (GLsizei)lightID, gpuLight->depthTexID);

		// TODO: Use string view
		GLuint lightSpaceLoc = glGetUniformLocation(m_program->m_shaderProgram, std::format(iLightFormat, "lightSpace").c_str());
		glUniformMatrix4fv(lightSpaceLoc, 1, GL_FALSE, light->m_lightSpace.data);

		GLuint pointLoc = glGetUniformLocation(m_program->m_shaderProgram, std::format(iLightFormat, "isPoint").c_str());
		glUniform1i(pointLoc, light->m_isPoint);

		GLuint posLoc = glGetUniformLocation(m_program->m_shaderProgram, std::format(iLightFormat, "position").c_str());
		glUniform3fv(posLoc, 1, light->m_position.data);

		GLuint diffLoc = glGetUniformLocation(m_program->m_shaderProgram, std::format(iLightFormat, "diffuse").c_str());
		glUniform3fv(diffLoc, 1, light->m_diffuse.data);

		GLuint ambientLoc = glGetUniformLocation(m_program->m_shaderProgram, std::format(iLightFormat, "ambient").c_str());
		glUniform3fv(ambientLoc, 1, light->m_ambient.data);

		GLuint specularLoc = glGetUniformLocation(m_program->m_shaderProgram, std::format(iLightFormat, "specular").c_str());
		glUniform3fv(specularLoc, 1, light->m_specular.data);

		lightID++;

		lightIt = std::next(m_lights.begin(), lightID);
	}

	for (ModelRenderer* modelRdr : m_modelRenderers)
	{
		if (!modelRdr->m_isVisible)
			continue;

		CCMaths::Matrix4 modelMat = modelRdr->m_transform->GetWorldMatrix();
		glUniformMatrix4fv(glGetUniformLocation(m_program->m_shaderProgram, "uModel"), 1, GL_FALSE, modelMat.data);


		Model* model = modelRdr->m_model.get();

		if (!model)
			continue;

		if (Material* material = model->m_material.get())
		{

			glUniform3fv(glGetUniformLocation(m_program->m_shaderProgram, "uMaterial.ambientCol"), 1, material->m_ambient.data);
			glUniform3fv(glGetUniformLocation(m_program->m_shaderProgram, "uMaterial.diffuseCol"), 1, material->m_diffuse.data);
			glUniform3fv(glGetUniformLocation(m_program->m_shaderProgram, "uMaterial.specularCol"), 1, material->m_specular.data);
			glUniform3fv(glGetUniformLocation(m_program->m_shaderProgram, "uMaterial.emissiveCol"), 1, material->m_emissive.data);
			glUniform1f(glGetUniformLocation(m_program->m_shaderProgram, "uMaterial.shininess"), material->m_shininess);

			if (Texture* albedoTexture = material->m_textures[ETextureType::ALBEDO].get())
			{
				if (auto gpuAlbedoTexture = static_cast<TextureGenerator::GPUTextureBasic*>(albedoTexture->m_gpuTexture.get()))
				{
					glBindTextureUnit(0, gpuAlbedoTexture->ID);
				}
			}

			if (Texture* normalMap = material->m_textures[ETextureType::NORMAL_MAP].get())
			{
				if (auto gpuNormalMap = static_cast<TextureGenerator::GPUTextureBasic*>(normalMap->m_gpuTexture.get()))
				{
					glBindTextureUnit(1, gpuNormalMap->ID);
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
