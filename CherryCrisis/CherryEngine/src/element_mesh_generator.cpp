#include <pch.hpp>

#include "element_mesh_generator.hpp"


ElementMeshGenerator::GPUMeshBasic::~GPUMeshBasic()
{
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
}

bool ElementMeshGenerator::Generate(Mesh* toGenerate)
{
	if (!toGenerate)
		return false;

	if (toGenerate->m_gpuMesh)
		return true;

	auto gpuMesh = std::make_unique<GPUMeshBasic>();

	// Generate EBO
	{
		gpuMesh->indicesCount = (GLsizei)toGenerate->m_indices.size();

		glCreateBuffers(1, &gpuMesh->EBO);
		glNamedBufferData(gpuMesh->EBO, gpuMesh->indicesCount * sizeof(unsigned int), toGenerate->m_indices.data(), GL_STATIC_DRAW);
	}

	// Generate VBO
	{
		size_t verticesCount = toGenerate->m_vertices.size();

		glCreateBuffers(1, &gpuMesh->VBO);
		glNamedBufferData(gpuMesh->VBO, verticesCount * sizeof(Vertex), toGenerate->m_vertices.data(), GL_STATIC_DRAW);
	}

	toGenerate->ClearData();

	// Generate VAO and link buffers
	{
		glCreateVertexArrays(1, &gpuMesh->VAO);
		glVertexArrayVertexBuffer(gpuMesh->VAO, 0, gpuMesh->VBO, 0, sizeof(Vertex));

		glVertexArrayElementBuffer(gpuMesh->VAO, gpuMesh->EBO);

		glEnableVertexArrayAttrib(gpuMesh->VAO, 0);
		glEnableVertexArrayAttrib(gpuMesh->VAO, 1);
		glEnableVertexArrayAttrib(gpuMesh->VAO, 2);
										 
		glVertexArrayAttribFormat(gpuMesh->VAO, 0, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, position));
		glVertexArrayAttribFormat(gpuMesh->VAO, 1, 2, GL_FLOAT, GL_FALSE, offsetof(Vertex, uv));
		glVertexArrayAttribFormat(gpuMesh->VAO, 2, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, normal));

		glVertexArrayAttribBinding(gpuMesh->VAO, 0, 0);
		glVertexArrayAttribBinding(gpuMesh->VAO, 1, 0);
		glVertexArrayAttribBinding(gpuMesh->VAO, 2, 0);
	}

	toGenerate->m_gpuMesh = std::move(gpuMesh);

	return true;
}

bool ElementMeshGenerator::Destroy(Mesh* toDestroy)
{
	GPUMeshBasic* gpuMesh = static_cast<GPUMeshBasic*>(toDestroy->m_gpuMesh.get());

	return gpuMesh;
}