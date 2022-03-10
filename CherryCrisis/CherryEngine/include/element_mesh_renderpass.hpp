#pragma once

#include <glad/gl.h>

#include "renderpass_interface.hpp"

#include "mesh.hpp"

class ElementMeshRenderPass : public ARenderPass
{
protected:
	struct GPUMeshBasic : GPUMesh
	{
		GLuint VAO = 0u, VBO = 0u, EBO = 0u;
	};

public:
	ElementMeshRenderPass(const char* pipelineName, const char* vert, const char* frag)
		: ARenderPass(pipelineName, vert, frag) { }

	int Generate(Mesh* toGenerate);
};