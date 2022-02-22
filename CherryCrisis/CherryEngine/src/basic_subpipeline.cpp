#include "basic_subpipeline.hpp"

#include "model_renderer.hpp"
#include "transform.hpp"
#include "model.hpp"

BasicSubPipeline::BasicSubPipeline(const char* name)
	: ASubPipeline(name, "../Assets/basicShader.vert", "../Assets/basicShader.frag")
{

}

template <>
int BasicSubPipeline::Generate(Light* toGenerate)
{
	m_lights.insert(toGenerate);

	return 1;
}

template <>
int BasicSubPipeline::Generate(ModelRenderer* toGenerate)
{
	Model* model = toGenerate->m_model.get();

	if (!model)
		return -1;

	// Generate GPU mesh
	{
		if (Mesh* mesh = model->m_mesh.get())
		{
			Generate(mesh);

			m_modelRenderers.insert(toGenerate);
		}
	}

	// Generate GPU textures
	{
		if (Material* material = model->m_material.get())
			Generate(material);
	}

	return 1;
}

template <>
int BasicSubPipeline::Generate(Mesh* toGenerate)
{
	if (!toGenerate)
		return -1;

	if (toGenerate->m_gpuMesh)
		return 0;

	GPUMeshBasic gpuMesh;

	// Generate EBO
	{
		size_t indicesCount = toGenerate->m_indices.size();

		glCreateBuffers(1, &gpuMesh.EBO);
		glNamedBufferData(gpuMesh.EBO, indicesCount * sizeof(unsigned int), toGenerate->m_indices.data(), GL_STATIC_DRAW);
	}

	// Generate VBO
	{
		size_t verticesCount = toGenerate->m_vertices.size();

		glCreateBuffers(1, &gpuMesh.VBO);
		glNamedBufferData(gpuMesh.VBO, verticesCount * sizeof(Vertex), toGenerate->m_vertices.data(), GL_STATIC_DRAW);
	}

	// Generate VAO and link buffers
	{
		glCreateVertexArrays(1, &gpuMesh.VAO);
		glVertexArrayVertexBuffer(gpuMesh.VAO, 0, gpuMesh.VBO, 0, sizeof(Vertex));

		glVertexArrayElementBuffer(gpuMesh.VAO, gpuMesh.EBO);

		glEnableVertexArrayAttrib(gpuMesh.VAO, 0);
		glEnableVertexArrayAttrib(gpuMesh.VAO, 1);
		glEnableVertexArrayAttrib(gpuMesh.VAO, 2);

		glVertexArrayAttribFormat(gpuMesh.VAO, 0, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, position));
		glVertexArrayAttribFormat(gpuMesh.VAO, 1, 2, GL_FLOAT, GL_FALSE, offsetof(Vertex, uv));
		glVertexArrayAttribFormat(gpuMesh.VAO, 2, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, normal));

		glVertexArrayAttribBinding(gpuMesh.VAO, 0, 0);
		glVertexArrayAttribBinding(gpuMesh.VAO, 1, 0);
		glVertexArrayAttribBinding(gpuMesh.VAO, 2, 0);
	}

	toGenerate->m_gpuMesh = new GPUMeshBasic(gpuMesh);

	return 1;
}

template <>
int BasicSubPipeline::Generate(Material* toGenerate)
{
	if (!toGenerate)
		return -1;

	// Albedo texture
	if (Texture* albedoTexture = toGenerate->albedoTex.get())
		Generate(albedoTexture);

	return 1;
}

template <>
int BasicSubPipeline::Generate(Texture* toGenerate)
{
	if (!toGenerate)
		return -1;

	if (toGenerate->m_gpuTexture)
		return 0;

	GPUTextureBasic gpuTexture;

	glCreateTextures(GL_TEXTURE_2D, 1, &gpuTexture.ID);

	glTextureParameteri(gpuTexture.ID, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTextureParameteri(gpuTexture.ID, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTextureParameteri(gpuTexture.ID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTextureParameteri(gpuTexture.ID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	if (!toGenerate->GetData() || toGenerate->GetWidth() <= 0 || toGenerate->GetHeight() <= 0)
		return -1;

	glTextureStorage2D(gpuTexture.ID, 1, GL_RGBA8, toGenerate->GetWidth(), toGenerate->GetHeight());
	glTextureSubImage2D(gpuTexture.ID, 0, 0, 0, toGenerate->GetWidth(), toGenerate->GetHeight(), GL_RGBA, GL_UNSIGNED_BYTE, toGenerate->GetData());

	glGenerateTextureMipmap(gpuTexture.ID);

	toGenerate->m_gpuTexture = new GPUTextureBasic(gpuTexture);

	return 1;
}

template <>
void BasicSubPipeline::Remove(ModelRenderer* toGenerate)
{
	m_modelRenderers.erase(toGenerate);
}

template <>
void BasicSubPipeline::Remove(Light* toGenerate)
{
	m_lights.erase(toGenerate);
}

void BasicSubPipeline::Execute()
{
	glEnable(GL_DEPTH_TEST);
	glUseProgram(m_program->m_shaderProgram);

	CCMaths::Matrix4 viewProj = Matrix4::Perspective(CCMaths::ToRadians(60.f), 4.f / 3.f, 0.1f, 100.f);
	glUniformMatrix4fv(glGetUniformLocation(m_program->m_shaderProgram, "uViewProjection"), 1, false, viewProj.data);

	size_t lightID = 0u;
	for (Light* light : m_lights)
	{
		std::string count = "[" + std::to_string(lightID) + "]";
		std::string lightIdentifier = "uLights" + count;

		std::string posString = lightIdentifier + ".position";
		GLuint posLoc = glGetUniformLocation(m_program->m_shaderProgram, posString.c_str());
		glUniform4fv(posLoc, 1, light->m_position.data);

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
		glUniformMatrix4fv(glGetUniformLocation(m_program->m_shaderProgram, "uModel"), 1, false, modelMat.data);

		float albedo[3] = { 1.0f, 1.0f, 1.0f };
		glUniform3fv(glGetUniformLocation(m_program->m_shaderProgram, "uMaterial.albedoColor"), 1, albedo);

		Model* model = modelRdr->m_model.get();

		if (!model)
			continue;

		if (Material* material = model->m_material.get(); material)
		{
			if (Texture* albedoTexture = material->albedoTex.get(); albedoTexture)
			{
				if (auto gpuAlbedoTexture = static_cast<GPUTextureBasic*>(albedoTexture->m_gpuTexture); gpuAlbedoTexture)
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
		glDrawElements(GL_TRIANGLES, mesh->m_indices.size(), GL_UNSIGNED_INT, nullptr);
	}

	glUseProgram(0);

	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}