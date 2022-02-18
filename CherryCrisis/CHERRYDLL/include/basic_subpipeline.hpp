#pragma once

#include "subpipeline_interface.hpp"

#include "model_renderer.hpp"
#include "model.hpp"
#include "mesh.hpp"

#include <glad/gl.h>

struct GPUMeshBasic : GPUMesh
{
	GLuint VAO = 0u;
	GLuint VBO = 0u;
	GLuint EBO = 0u;
};

class BasicSubPipeline : public ISubPipeline
{
public:
	template <typename RendererT>
	static void Generate(RendererT* toGenerate) { }

	template <typename RendererT>
	static void Consume(RendererT* toGenerate) { }

	template <>
	static void Generate(ModelRenderer* toGenerate)
	{
		if (toGenerate->m_gpuMesh)
			return;

		GPUMeshBasic gpuMesh;

		if (!toGenerate->m_model)
			return;

		Mesh* mesh = toGenerate->m_model->m_mesh.get();

		if (!mesh)
			return;

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
	}

	template <>
	static void Consume(ModelRenderer* toGenerate)
	{
		Mesh* mesh = toGenerate->m_model->m_mesh.get();
		GPUMeshBasic* gpuMesh = static_cast<GPUMeshBasic*>(toGenerate->m_gpuMesh);

		glBindVertexArray(gpuMesh->VAO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gpuMesh->EBO);
		glDrawElements(GL_TRIANGLES, mesh->m_indices.size(), GL_FLOAT, nullptr);

		glBindVertexArray(0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
};