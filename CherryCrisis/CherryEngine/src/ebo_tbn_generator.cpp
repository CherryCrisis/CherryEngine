#include "pch.hpp"

#include "ebo_tbn_generator.hpp"

#include "mesh.hpp"

bool ElementTBNGenerator::Generate(Mesh* toGenerate)
{
	if (!ElementMeshGenerator::Generate(toGenerate))
		return false;

	GPUMeshBasic* gpuMesh = static_cast<GPUMeshBasic*>(toGenerate->m_gpuMesh.get());

	glEnableVertexArrayAttrib(gpuMesh->VAO, 3);
	glEnableVertexArrayAttrib(gpuMesh->VAO, 4);

	glVertexArrayAttribFormat(gpuMesh->VAO, 3, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, tangent));
	glVertexArrayAttribFormat(gpuMesh->VAO, 4, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, bitangent));

	glVertexArrayAttribBinding(gpuMesh->VAO, 3, 0);
	glVertexArrayAttribBinding(gpuMesh->VAO, 4, 0);

	return true;
}