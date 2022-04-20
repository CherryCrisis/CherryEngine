#include "pch.hpp"

#include "basic_renderpass.hpp"

#include "framebuffer.hpp"

#include "camera.hpp"
#include "model_renderer.hpp"
#include "transform.hpp"
#include "model.hpp"

#include "shadow_renderpass.hpp"
#include "viewer.hpp"

BasicRenderPass::GPUTextureBasic::~GPUTextureBasic()
{
	glDeleteTextures(1, &ID);
}

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
		if (!ElementMeshGenerator::Generate(model->m_mesh.get()))
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
int BasicRenderPass::Subscribe(Material* toGenerate)
{
	if (!toGenerate)
		return -1;

	// Albedo texture
	if (Texture* albedoTexture = toGenerate->textures[ETextureType::ALBEDO].get())
		Subscribe(albedoTexture);

	// Normal texture
	if (Texture* normalMap = toGenerate->textures[ETextureType::NORMAL_MAP].get())
		Subscribe(normalMap);

	return 1;
}

template <>
int BasicRenderPass::Subscribe(Texture* toGenerate)
{
	if (!toGenerate)
		return -1;

	if (toGenerate->m_gpuTexture)
		return 0;

	auto gpuTexture = std::make_unique<GPUTextureBasic>();

	glCreateTextures(GL_TEXTURE_2D, 1, &gpuTexture->ID);

	glTextureParameteri(gpuTexture->ID, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTextureParameteri(gpuTexture->ID, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTextureParameteri(gpuTexture->ID, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTextureParameteri(gpuTexture->ID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	if (!toGenerate->GetData() || toGenerate->GetWidth() <= 0 || toGenerate->GetHeight() <= 0)
		return -1;

	glTextureStorage2D(gpuTexture->ID, 1, GL_RGBA8, toGenerate->GetWidth(), toGenerate->GetHeight());
	glTextureSubImage2D(gpuTexture->ID, 0, 0, 0, toGenerate->GetWidth(), toGenerate->GetHeight(), GL_RGBA, GL_UNSIGNED_BYTE, toGenerate->GetData());

	glGenerateTextureMipmap(gpuTexture->ID);

	toGenerate->m_gpuTexture = std::move(gpuTexture);

	return 1;
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

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	glCullFace(GL_BACK);

	glUseProgram(m_program->m_shaderProgram);
	glClearColor(0.f, 0.f, 0.f, 1.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	float aspect = (float)framebuffer.width / (float)framebuffer.height;
	viewer->m_projectionMatrix = Matrix4::Perspective(DEG2RAD * 90.f, aspect, 0.01f, 200.f);

	CCMaths::Matrix4 viewProjection = viewer->m_projectionMatrix * viewer->m_viewMatrix;
	glUniformMatrix4fv(glGetUniformLocation(m_program->m_shaderProgram, "uViewProjection"), 1, GL_FALSE, viewProjection.data);

	glUniform3fv(glGetUniformLocation(m_program->m_shaderProgram, "uViewPosition"), 1, (-viewer->m_viewMatrix.position).data);

	size_t lightID = 0u;
	const char* lightFormat = "uLights[{}]";
	for (Light* light : m_lights)
	{
		auto* gpuLight = static_cast<ShadowRenderPass::GPUShadowLight*>(light->m_gpuLight.get());

		if (!gpuLight)
		{
			lightID++;
			continue;
		}

		// TODO: Optimize, compute one time
		CCMaths::Matrix4 lightView = CCMaths::Matrix4::LookAt(light->m_position);
		CCMaths::Matrix4 ortho = CCMaths::Matrix4::Transpose(CCMaths::Matrix4::Orthographic(-10.f, 10.f, -10.f, 10.f, -50.f, 20.f));
		CCMaths::Matrix4 lightSpace = ortho * lightView;

		glUniform1i(glGetUniformLocation(m_program->m_shaderProgram, "uShadowMaps") + lightID, 3 + lightID);
		glBindTextureUnit(3 + lightID, gpuLight->depthTexID);

		// TODO: Use string view
		std::string iLightFormat = std::format(lightFormat, lightID) + ".{}";
		GLuint lightSpaceLoc = glGetUniformLocation(m_program->m_shaderProgram, std::format(iLightFormat, "lightSpace").c_str());
		glUniformMatrix4fv(lightSpaceLoc, 1, GL_FALSE, lightSpace.data);

		GLuint enableLoc = glGetUniformLocation(m_program->m_shaderProgram, std::format(iLightFormat, "isEnabled").c_str());
		glUniform1i(enableLoc, true);

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

			if (Texture* albedoTexture = material->textures[ETextureType::ALBEDO].get())
			{
				if (auto gpuAlbedoTexture = static_cast<GPUTextureBasic*>(albedoTexture->m_gpuTexture.get()))
				{
					glBindTextureUnit(0, gpuAlbedoTexture->ID);
				}
			}

			if (Texture* normalMap = material->textures[ETextureType::NORMAL_MAP].get())
			{
				if (auto gpuNormalMap = static_cast<GPUTextureBasic*>(normalMap->m_gpuTexture.get()))
				{
					glBindTextureUnit(1, gpuNormalMap->ID);
				}
			}
		}

		Mesh* mesh = model->m_mesh.get();

		if (!mesh)
			continue;

		GPUMeshBasic* gpuMesh = static_cast<GPUMeshBasic*>(mesh->m_gpuMesh.get());

		if (!gpuMesh)
			continue;

		glBindVertexArray(gpuMesh->VAO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gpuMesh->EBO);
		glDrawElements(GL_TRIANGLES, (GLsizei)mesh->m_indices.size(), GL_UNSIGNED_INT, nullptr);
	}

	glUseProgram(0);

	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}
