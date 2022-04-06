#include "pch.hpp"

#include "basic_renderpass.hpp"

#include "camera_component.hpp"
#include "model_renderer.hpp"
#include "transform.hpp"
#include "model.hpp"

BasicRenderPass::BasicRenderPass(const char* name)
	: ARenderPass(name, "Assets/basicShader.vert", "Assets/basicShader.frag")
{
	if (m_program)
		m_callExecute = CCCallback::BindCallback(&BasicRenderPass::Execute, this);
}

template <>
int BasicRenderPass::Generate(Light* toGenerate)
{
	if (!toGenerate)
		return -1;

	m_lights.insert(toGenerate);

	return 1;
}

template <>
int BasicRenderPass::Generate(CameraComponent* toGenerate)
{
	if (!toGenerate)
		return -1;

	m_camera = toGenerate;
	return 1;
}

template <>
int BasicRenderPass::Generate(ModelRenderer* toGenerate)
{
	Model* model = toGenerate->m_model.get();

	if (!model)
		return -1;

	// Generate GPU mesh
	{
		if (ElementMeshGenerator::Generate(model->m_mesh.get()) == -1)
			return -1;

		m_modelRenderers.insert(toGenerate);
	}

	// Generate GPU textures
	{
		if (Material* material = model->m_material.get())
			Generate(material);
	}

	return 1;
}

template <>
int BasicRenderPass::Generate(Material* toGenerate)
{
	if (!toGenerate)
		return -1;

	// Albedo texture
	if (Texture* albedoTexture = toGenerate->textures["albedo"].get())
		Generate(albedoTexture);

	return 1;
}

template <>
int BasicRenderPass::Generate(Texture* toGenerate)
{
	if (!toGenerate)
		return -1;

	if (toGenerate->m_gpuTexture)
		return 0;

	GPUTextureBasic gpuTexture;

	glCreateTextures(GL_TEXTURE_2D, 1, &gpuTexture.ID);
	
	glBindTexture(GL_TEXTURE_2D, gpuTexture.ID);

	//glTextureStorage2D(gpuTexture.ID, 1, GL_RGBA8, toGenerate->GetWidth(), toGenerate->GetHeight());
	//glTextureSubImage2D(gpuTexture.ID, 0, 0, 0, toGenerate->GetWidth(), toGenerate->GetHeight(), GL_COMPRESSED_RGBA, GL_UNSIGNED_BYTE, toGenerate->GetData());

	//glTexImage2D(GL_TEXTURE_2D, 0, GL_COMPRESSED_RGBA, toGenerate->GetWidth(), toGenerate->GetHeight(),
	//	0, GL_BGR, GL_UNSIGNED_BYTE, toGenerate->GetData());

	//int compressed;
	//glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_COMPRESSED, &compressed);

	//int internalFormat;
	//int compressed_size;
	//unsigned char* img;

	///* if the compression has been successful */
	//if (compressed == GL_TRUE)
	//{
	//	glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_INTERNAL_FORMAT,
	//		&internalFormat);
	//	glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_COMPRESSED_IMAGE_SIZE,
	//		&compressed_size);
	//	img = (unsigned char*)malloc(compressed_size * sizeof(unsigned char));
	//	glGetCompressedTexImage(GL_TEXTURE_2D, 0, img);
	//	//SaveTexture(width, height, compressed_size, img, internalFormat, 0);
	//}

	glTextureParameteri(gpuTexture.ID, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTextureParameteri(gpuTexture.ID, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTextureParameteri(gpuTexture.ID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTextureParameteri(gpuTexture.ID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	if (!toGenerate->GetData() || toGenerate->GetWidth() <= 0 || toGenerate->GetHeight() <= 0)
		return -1;

	//glTextureStorage2D(gpuTexture.ID, 1, GL_RGBA8, toGenerate->GetWidth(), toGenerate->GetHeight());
	//glTextureSubImage2D(gpuTexture.ID, 0, 0, 0, toGenerate->GetWidth(), toGenerate->GetHeight(), GL_COMPRESSED_RGBA, GL_UNSIGNED_BYTE, toGenerate->GetData());

	/*https://www.oldunreal.com/editing/s3tc/ARB_texture_compression.pdf*/
	int blockSize = 8; //compression level
	int size = ((toGenerate->GetWidth()) / 4) * ((toGenerate->GetHeight()) / 4) * blockSize;
	glCompressedTexImage2D(GL_TEXTURE_2D, 0, GL_COMPRESSED_RGBA_BPTC_UNORM, toGenerate->GetWidth(), toGenerate->GetHeight(), 0, size, toGenerate->GetData());
	
	glGenerateTextureMipmap(gpuTexture.ID);

	toGenerate->m_gpuTexture = new GPUTextureBasic(gpuTexture);

	return 1;
}

template <>
void BasicRenderPass::Remove(ModelRenderer* toGenerate)
{
	m_modelRenderers.erase(toGenerate);
}

template <>
void BasicRenderPass::Remove(Light* toGenerate)
{
	m_lights.erase(toGenerate);
}

template <>
void BasicRenderPass::Remove(CameraComponent* toGenerate)
{
	m_camera = nullptr;
}

void BasicRenderPass::Execute(const float& x, const float& y)
{
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	glCullFace(GL_BACK);

	glUseProgram(m_program->m_shaderProgram);

	if (m_camera)
	{
		// TODO: Change this
		m_camera->m_camera.aspect = x / y;
		CCMaths::Matrix4 projection = Matrix4::Perspective(m_camera->m_camera.fovY, m_camera->m_camera.aspect, m_camera->m_camera.near, m_camera->m_camera.far);
		CCMaths::Matrix4 view = Matrix4::RotateZXY(-m_camera->m_camera.rotation) * Matrix4::Translate(m_camera->m_camera.position);

		CCMaths::Matrix4 viewProjection = projection * view;
		glUniformMatrix4fv(glGetUniformLocation(m_program->m_shaderProgram, "uViewProjection"), 1, GL_FALSE, viewProjection.data);
	}

	size_t lightID = 0u;
	for (Light* light : m_lights)
	{
		std::string count = "[" + std::to_string(lightID) + "]";
		std::string lightIdentifier = "uLights" + count;

		std::string pointString = lightIdentifier + ".isPoint";
		GLuint pointLoc = glGetUniformLocation(m_program->m_shaderProgram, pointString.c_str());
		glUniform1i(pointLoc, light->m_isPoint);

		std::string posString = lightIdentifier + ".position";
		GLuint posLoc = glGetUniformLocation(m_program->m_shaderProgram, posString.c_str());
		glUniform3fv(posLoc, 1, light->m_position.data);

		std::string diffuseString = lightIdentifier + ".diffuse";
		GLuint diffLoc = glGetUniformLocation(m_program->m_shaderProgram, diffuseString.c_str());
		glUniform3fv(diffLoc, 1, light->m_diffuse.data);

		lightID++;
	}

	for (ModelRenderer* modelRdr : m_modelRenderers)
	{
		if (!modelRdr->m_isVisible)
			continue;

		CCMaths::Matrix4 modelMat = modelRdr->m_transform->GetWorldMatrix();
		glUniformMatrix4fv(glGetUniformLocation(m_program->m_shaderProgram, "uModel"), 1, GL_FALSE, modelMat.data);

		float albedo[3] = { 1.0f, 1.0f, 1.0f };
		glUniform3fv(glGetUniformLocation(m_program->m_shaderProgram, "uMaterial.albedoColor"), 1, albedo);

		Model* model = modelRdr->m_model.get();

		if (!model)
			continue;

		if (Material* material = model->m_material.get())
		{
			if (Texture* albedoTexture = material->textures["albedo"].get())
			{
				if (auto gpuAlbedoTexture = static_cast<GPUTextureBasic*>(albedoTexture->m_gpuTexture))
					glBindTextureUnit(0, gpuAlbedoTexture->ID);
			}
		}

		Mesh* mesh = model->m_mesh.get();

		if (!mesh)
			continue;

		GPUMeshBasic* gpuMesh = static_cast<GPUMeshBasic*>(mesh->m_gpuMesh);

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