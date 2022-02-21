#include "basic_subpipeline.hpp"

BasicSubPipeline::BasicSubPipeline(const char* name)
	: ASubPipeline(name, "../Assets/basicShader.vert", "../Assets/basicShader.frag")
{

}

template <>
void BasicSubPipeline::Generate(ModelRenderer* toGenerate)
{
	if (toGenerate->m_gpuMesh)
		return;


	Model* model = toGenerate->m_model.get();

	if (!model)
		return;

	// Generate GPU mesh
	{
		Mesh* mesh = model->m_mesh.get();

		if (!mesh)
			return;

		GPUMeshBasic gpuMesh;

		// Generate EBO
		{
			size_t indicesCount = mesh->m_indices.size();

			glGenBuffers(1, &gpuMesh.EBO);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gpuMesh.EBO);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicesCount * sizeof(unsigned int), mesh->m_indices.data(), GL_STATIC_DRAW);
		}

		// Generate VBO
		{
			size_t verticesCount = mesh->m_vertices.size();

			glGenBuffers(1, &gpuMesh.VBO);
			glBindBuffer(GL_ARRAY_BUFFER, gpuMesh.VBO);
			glBufferData(GL_ARRAY_BUFFER, verticesCount * sizeof(Vertex), mesh->m_vertices.data(), GL_STATIC_DRAW);
		}

		// Generate VAO and link buffers
		{
			glGenVertexArrays(1, &gpuMesh.VAO);
			glBindVertexArray(gpuMesh.VAO);

			glBindBuffer(GL_ARRAY_BUFFER, gpuMesh.VBO);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gpuMesh.EBO);

			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, uv));
			glEnableVertexAttribArray(2);
			glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
		}

		toGenerate->m_gpuMesh = new GPUMeshBasic(gpuMesh);

		m_modelRenderers.insert(toGenerate);
	}

	// Generate GPU textures
	{
		Material* material = model->m_material.get();

		if (!material)
			return;

		Texture* albedoTexture = material->albedoTexture.get();

		if (!albedoTexture)
			return;

		if (albedoTexture->m_gpuTexture)
			return;

		// Albedo texture
		GPUTextureBasic gpuTexture;
		glGenTextures(1, &gpuTexture.ID);
		glBindTexture(GL_TEXTURE_2D, gpuTexture.ID);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, albedoTexture->GetWidth(), albedoTexture->GetHeight(), 0, GL_RGBA, GL_UNSIGNED_BYTE, albedoTexture->GetData());

		glGenerateMipmap(GL_TEXTURE_2D);

		glBindTexture(GL_TEXTURE_2D, 0);

		albedoTexture->m_gpuTexture = new GPUTextureBasic(gpuTexture);
	}

}

template <>
void BasicSubPipeline::Remove(ModelRenderer* toGenerate)
{
	m_modelRenderers.erase(toGenerate);
}

void BasicSubPipeline::Execute()
{
	glEnable(GL_DEPTH_TEST);
	glUseProgram(m_program->m_shaderProgram);

	CCMaths::Matrix4 viewProj = Matrix4::Perspective(CCMaths::ToRadians(60.f), 4.f / 3.f, 0.001f, 1000.f);
	glUniformMatrix4fv(glGetUniformLocation(m_program->m_shaderProgram, "uViewProjection"), 1, false, viewProj.data);

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
			if (Texture* albedoTexture = material->albedoTexture.get(); albedoTexture)
			{
				if (auto gpuAlbedoTexture = static_cast<GPUTextureBasic*>(albedoTexture->m_gpuTexture); gpuAlbedoTexture)
				{
					glActiveTexture(GL_TEXTURE0);
					glBindTexture(GL_TEXTURE_2D, gpuAlbedoTexture->ID);
				}
			}
		}

		Mesh* mesh = model->m_mesh.get();

		if (!mesh)
			continue;

		GPUMeshBasic* gpuMesh = static_cast<GPUMeshBasic*>(modelRdr->m_gpuMesh);

		if (!gpuMesh)
			continue;

		glBindVertexArray(gpuMesh->VAO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gpuMesh->EBO);
		glDrawElements(GL_TRIANGLES, mesh->m_indices.size(), GL_UNSIGNED_INT, nullptr);
	}

	glUseProgram(0);

	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}