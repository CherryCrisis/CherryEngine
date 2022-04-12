#include "pch.hpp"

#include "basic_renderpass.hpp"

#include "framebuffer.hpp"

#include "camera.hpp"
#include "model_renderer.hpp"
#include "transform.hpp"
#include "model.hpp"

BasicRenderPass::GPUTextureBasic::~GPUTextureBasic()
{
	glDeleteTextures(1, &ID);
}

BasicRenderPass::BasicRenderPass(const char* name)
	: ARenderPass(name, "Assets/basicShader.vert", "Assets/basicShader.frag")
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
	if (!toGenerate)
		return -1;

	m_lights.insert(toGenerate);

	return 1;
}

template <>
int BasicRenderPass::Subscribe(Camera* toGenerate)
{
	if (!toGenerate)
		return -1;

	m_camera = toGenerate;
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

	glTextureStorage2D(gpuTexture->ID, 1, GL_RGBA8, toGenerate->GetWidth(), toGenerate->GetHeight());
	glTextureSubImage2D(gpuTexture->ID, 0, 0, 0, toGenerate->GetWidth(), toGenerate->GetHeight(), GL_RGBA, GL_UNSIGNED_BYTE, toGenerate->GetData());

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

template <>
void BasicRenderPass::Unsubscribe(Camera* toGenerate)
{
	m_camera = nullptr;
}

void BasicRenderPass::Execute(Framebuffer& framebuffer, Camera& camera)
{
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	glCullFace(GL_BACK);

	glUseProgram(m_program->m_shaderProgram);

	// TODO: Change this
	camera.aspect = (float)framebuffer.width / (float)framebuffer.height;
	CCMaths::Matrix4 projection = Matrix4::Perspective(camera.fovY, camera.aspect, camera.near, camera.far);
	CCMaths::Matrix4 view = Matrix4::RotateZXY(-camera.rotation) * Matrix4::Translate(-camera.position);

	CCMaths::Matrix4 viewProjection = projection * view;
	glUniformMatrix4fv(glGetUniformLocation(m_program->m_shaderProgram, "uViewProjection"), 1, GL_FALSE, viewProjection.data);

	glUniform3fv(glGetUniformLocation(m_program->m_shaderProgram, "uViewPosition"), GL_FALSE, camera.position.data);
	

	size_t lightID = 0u;
	const char* lightFormat = "uLights[{}]";
	for (Light* light : m_lights)
	{
		// TODO: Use string view
		std::string iLightFormat = std::format(lightFormat, lightID) + ".{}";
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
