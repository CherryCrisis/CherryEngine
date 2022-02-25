#pragma once

#include <glad/gl.h>

#include "subpipeline_interface.hpp"

#include "mesh.hpp"

class ElementMeshPipeline : public ASubPipeline
{
protected:
	struct GPUMeshBasic : GPUMesh
	{
		GLuint VAO = 0u, VBO = 0u, EBO = 0u;
	};

public:
	ElementMeshPipeline(const char* pipelineName, const char* vert, const char* frag)
		: ASubPipeline(pipelineName, vert, frag) { }

	int Generate(Mesh* toGenerate);
};